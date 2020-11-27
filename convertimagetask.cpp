#include "convertimagetask.h"

ConvertImageTask::ConvertImageTask(QObject *reciever,
                                   volatile bool *stopped,
                                   const QStringList &sourceFiles,
                                   const QString &targetType,
                                   const QString &path,
                                   const int &optimizationLevel,
                                   const int renderingIntent,
                                   QByteArray inProfileData,
                                   QByteArray outProfileData)
{
    this->reciever = reciever;
    this->stopped = stopped;
    this->sourceFiles = sourceFiles;
    this->targetType = targetType;
    this->path = path;

    this->optimizationLevel = optimizationLevel;

    this->renderingIntent = renderingIntent;

    this->inProfileData = inProfileData;
    this->outProfileData = outProfileData;
}

void ConvertImageTask::run()
{

    QString threadStartMessage = "Thread called with: "
                                 + QString::number(sourceFiles.size()) + " files.";

    emit threadUpdate(threadStartMessage);

    QString processedPath = path + "/_processed";

    // loop through set
    QStringListIterator it(this->sourceFiles);

    while(it.hasNext()){

        QString fileName = it.next();
        QString filePath = path + "/" + fileName;

        emit threadUpdate("Processing: " + fileName);

        // save name
        QString outName = processedPath + "/" + fileName;

        QString extension = fileName.split(".").last();

        // jpeg
        if(extension.toLower().contains("jpeg")
            || extension.toLower().contains("jpg")){

        QImage imin(filePath);

        int wt = imin.width();
        int ht = imin.height();


        // the single most important optimzation!
        if(wt > 1200 || ht > 1200){

            imin = imin.scaled(QSize(1200, 1200), Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);

            wt = imin.width();
            ht = imin.height();

        }

        QImage *imout;

        // littlecms

        cmsHPROFILE hInProfile, hOutProfile;
        cmsHTRANSFORM hTransform;

        // memory function calls in imagemanager.cpp

        hInProfile = cmsOpenProfileFromMem(inProfileData.data(), inProfileData.size());

        hOutProfile = cmsOpenProfileFromMem(outProfileData.data(), outProfileData.size());

        int intent = 0;

        switch(renderingIntent){
        case 0 :
            intent = INTENT_ABSOLUTE_COLORIMETRIC;
            break;
        case 1 :
            intent = INTENT_PERCEPTUAL;
            break;
        case 2 :
            intent = INTENT_RELATIVE_COLORIMETRIC;
            break;
        case 3 :
            intent = INTENT_SATURATION;
            break;
        }

        hTransform = cmsCreateTransform(hInProfile,
                                        TYPE_CMYK_8,
                                        hOutProfile,
                                        TYPE_RGB_8,
                                        intent, 0);

        cmsCloseProfile(hInProfile);
        cmsCloseProfile(hOutProfile);

        // create uchar to hold raw color data
        uchar *cmyk_temp = new uchar[(wt * ht) * 4];
        uchar *rgb_temp = new uchar[(wt * ht) * 3];

        int j = 0; int ju = 0;

        int loop_count = wt * ht;

        if(optimizationLevel == 0){

            // base code
            for(int y = 0; y < imin.height(); y++){
                for(int x = 0; x < imin.width(); x++){
                    QColor fp = imin.pixel(x, y);
                    int c = fp.convertTo(QColor::Cmyk).cyan();
                    int m = fp.convertTo(QColor::Cmyk).magenta();
                    int y = fp.convertTo(QColor::Cmyk).yellow();
                    int k = fp.convertTo(QColor::Cmyk).black();
                    cmyk_temp[j] = c;
                    cmyk_temp[j + 1] = m;
                    cmyk_temp[j + 2] = y;
                    cmyk_temp[j + 3] = k;
                    j += 4;
                }
            }

        } else if(optimizationLevel == 1){

            // optimization 1
            for(int y = 0; y < ht; y++){
                QRgb *scan = reinterpret_cast<QRgb*>(imin.scanLine(y));
                for(int i = 0; i < wt; ++i){
                    QColor fp = scan[i];
                    QColor c = fp.convertTo(QColor::Cmyk);
                    cmyk_temp[j] = c.cyan();
                    cmyk_temp[j + 1] = c.magenta();
                    cmyk_temp[j + 2] = c.yellow();
                    cmyk_temp[j + 3] = c.black();
                    j += 4;
                }
            }

        } else if(optimizationLevel == 2){

            // optimization 2
            QRgb *bit = reinterpret_cast<QRgb*>(imin.bits());
            for(int i = 0; i < wt * ht; ++i){
                QColor fp = bit[i];
                QColor c = fp.convertTo(QColor::Cmyk);
                cmyk_temp[j] = c.cyan();
                cmyk_temp[j + 1] = c.magenta();
                cmyk_temp[j + 2] = c.yellow();
                cmyk_temp[j + 3] = c.black();
                j += 4;
            }

        } else if(optimizationLevel == 3){

            // optimization 3
            uchar *bits = imin.bits();
            qreal c_c, c_m, c_y, c_k;
            for(int i = 0; i < wt * ht; i++){

                c_c = 1.0 - (bits[j+2] / qreal(255));
                c_m = 1.0 - (bits[j+1] / qreal(255));
                c_y = 1.0 - (bits[j] / qreal(255));

                c_k = qMin(c_c, qMin(c_m, c_y));

                if (!qFuzzyCompare(c_k,1)) {
                    c_c = (c_c - c_k) / (1.0 - c_k);
                    c_m = (c_m - c_k) / (1.0 - c_k);
                    c_y = (c_y - c_k) / (1.0 - c_k);
                }

                cmyk_temp[j] = c_c * 255;
                cmyk_temp[j + 1] = c_m * 255;
                cmyk_temp[j + 2] = c_y * 255;
                cmyk_temp[j + 3] = c_k * 255;

                j += 4;

            }

        } else if(optimizationLevel == 4){


            // optimzation 4
            uchar *bits = imin.bits();
            qreal c_c, c_m, c_y, c_k;
            for(int i = 0; i < wt * ht; i++){

                c_c = bits[j+2];
                c_m = bits[j+1];
                c_y = bits[j];

                c_c = 1.0 - (bits[j+2] / 255.0);
                c_m = 1.0 - (bits[j+1] / 255.0);
                c_y = 1.0 - (bits[j] / 255.0);

                c_k = qMin(c_c, qMin(c_m, c_y));

                if (c_k != 1) {
                    qreal onemin = 1.0 - c_k;
                    c_c = (c_c - c_k) / (onemin);
                    c_m = (c_m - c_k) / (onemin);
                    c_y = (c_y - c_k) / (onemin);
                } else {
                    c_c = 0;
                    c_m = 0;
                    c_y = 0;
                }

                cmyk_temp[j] = c_c * 255;
                cmyk_temp[j + 1] = c_m * 255;
                cmyk_temp[j + 2] = c_y * 255;
                cmyk_temp[j + 3] = c_k * 255;

                j += 4;

            }

        } else if(optimizationLevel == 5){

            // optimzation level 5

            uchar *bits = imin.bits();
            qreal c_c, c_m, c_y, c_k, c_c1, c_m1, c_y1, c_k1, onemin, onemin1;
            int l = wt * ht;
            int l2 = (wt / 2) * ht;

            int yu = wt % 2;

            if(wt % 2 == 0){

                for(int i = 0; i < l2; i++){

                    c_c = 1.0 - (bits[ju+2] / 255.0);
                    c_m = 1.0 - (bits[ju+1] / 255.0);
                    c_y = 1.0 - (bits[ju] / 255.0);
                    c_k = qMin(c_c, qMin(c_m, c_y));

                    c_c1 = 1.0 - (bits[ju+6] / 255.0);
                    c_m1 = 1.0 - (bits[ju+5] / 255.0);
                    c_y1 = 1.0 - (bits[ju+4] / 255.0);
                    c_k1 = qMin(c_c1, qMin(c_m1, c_y1));

                    if (c_k != 1) {
                        onemin = 1.0 - c_k;
                        c_c = (c_c - c_k) / (onemin);
                        c_m = (c_m - c_k) / (onemin);
                        c_y = (c_y - c_k) / (onemin);

                        cmyk_temp[ju] = c_c * 255;
                        cmyk_temp[ju + 1] = c_m * 255;
                        cmyk_temp[ju + 2] = c_y * 255;
                        cmyk_temp[ju + 3] = c_k * 255;

                    } else {

                        cmyk_temp[ju] = 0;
                        cmyk_temp[ju + 1] = 0;
                        cmyk_temp[ju + 2] = 0;
                        cmyk_temp[ju + 3] = c_k * 255;
                    }

                    if (c_k1 != 1) {
                        onemin1 = 1.0 - c_k1;
                        c_c1 = (c_c1 - c_k1) / (onemin1);
                        c_m1 = (c_m1 - c_k1) / (onemin1);
                        c_y1 = (c_y1 - c_k1) / (onemin1);

                        cmyk_temp[ju + 4] = c_c1 * 255;
                        cmyk_temp[ju + 5] = c_m1 * 255;
                        cmyk_temp[ju + 6] = c_y1 * 255;
                        cmyk_temp[ju + 7] = c_k1 * 255;

                    } else {

                        cmyk_temp[ju + 4] = 0;
                        cmyk_temp[ju + 5] = 0;
                        cmyk_temp[ju + 6] = 0;
                        cmyk_temp[ju + 7] = c_k1 * 255;
                    }

                    ju += 8;

                }
            } else {

                for(int i = 0; i < l; i++){

                    c_c = 1.0 - (bits[j+2] / 255.0);
                    c_m = 1.0 - (bits[j+1] / 255.0);
                    c_y = 1.0 - (bits[j] / 255.0);

                    c_k = qMin(c_c, qMin(c_m, c_y));

                    if (c_k != 1) {
                        qreal onemin = 1.0 - c_k;
                        c_c = (c_c - c_k) / (onemin);
                        c_m = (c_m - c_k) / (onemin);
                        c_y = (c_y - c_k) / (onemin);

                        cmyk_temp[j] = c_c * 255;
                        cmyk_temp[j + 1] = c_m * 255;
                        cmyk_temp[j + 2] = c_y * 255;
                        cmyk_temp[j + 3] = c_k * 255;

                        j += 4;

                    } else {

                        cmyk_temp[j] = 0;
                        cmyk_temp[j + 1] = 0;
                        cmyk_temp[j + 2] = 0;
                        cmyk_temp[j + 3] = c_k * 255;

                        j += 4;

                    }

                }
            }

        } else if(optimizationLevel == 6){

            // SSE 2

            j = 0;

            uchar *bits = imin.bits();

            __m128 v1;
            __m128 k1;

            __m128 m_255 = _mm_set_ps1(255.0f);

            __m128 m_1 = _mm_set_ps1(1.0f);

            float tmp1[4];;

            float tmp_1;

            for(int i = 0; i < wt * ht; i++){

                v1 = _mm_set_ps(1, bits[j], bits[j+1], bits[j+2]); // rev order!

                // fetch next data values
                //_mm_prefetch(&bits[j+16], _MM_HINT_T0);

                // divide by 255
                v1 = _mm_div_ps(v1, m_255);

                // subtract 1
                v1 = _mm_sub_ps(m_1, v1);

                // store to find min
                _mm_store_ps(tmp1, v1);

                // find min (_mm_min_ps)
                tmp_1 = qMin(tmp1[0], qMin(tmp1[1], tmp1[2]));

                if (!qFuzzyCompare(tmp1[3],1)) {

                    // subtract original k
                    v1 = _mm_sub_ps(v1, _mm_set_ps(tmp_1, tmp_1, tmp_1, tmp_1));

                    // subtract 1 from base k
                    float bk_min1 = 1.0 - tmp_1;

                    // divide by (cmy - k) - (1.0 - k)
                    v1 = _mm_div_ps(v1, _mm_set_ps(bk_min1, bk_min1, bk_min1, bk_min1));

                }

                // store to extract (prevents green cast)
                _mm_store_ps(tmp1, v1);

                k1 = _mm_set_ps(tmp_1, tmp1[2], 0, 0); // reverse

                // add fourth vector back in (k)
                v1 = _mm_shuffle_ps(v1, k1, _MM_SHUFFLE(3, 2, 1, 0)); // source, dest

                // multiply all by 255
                v1 = _mm_mul_ps(v1, m_255);

                // standard store
                _mm_store_ps(tmp1, v1); // save back to tmp buffer

                // mem write
                cmyk_temp[j] = tmp1[0]; // c
                cmyk_temp[j+1] = tmp1[1]; // m
                cmyk_temp[j+2] = tmp1[2]; // y
                cmyk_temp[j+3] = tmp1[3]; // k

                j+=4;

            }

        } else if(optimizationLevel == 7){

            // SSE 2 - unrolled vector

            j = 0;

            uchar *bits = imin.bits();

            __m128 v1, v2, v3, v4;
            __m128 k1, k2, k3, k4;

            __m128 m_255 = _mm_set_ps1(255.0f);

            __m128 m_1 = _mm_set_ps1(1.0f);

            float tmp1[4], tmp2[4], tmp3[4], tmp4[4];

            float tmp_1, tmp_2, tmp_3, tmp_4;

            for(int i = 0; i < wt * ht; i+=4){

                v1 = _mm_set_ps(1, bits[j], bits[j+1], bits[j+2]); // rev order!
                v2 = _mm_set_ps(1, bits[j+4], bits[j+5], bits[j+6]); // rev order!
                v3 = _mm_set_ps(1, bits[j+8], bits[j+9], bits[j+10]); // rev order!
                v4 = _mm_set_ps(1, bits[j+12], bits[j+13], bits[j+14]); // rev order!

                // fetch next data values
                //_mm_prefetch(&bits[j+16], _MM_HINT_T0);

                // divide by 255
                v1 = _mm_div_ps(v1, m_255);
                v2 = _mm_div_ps(v2, m_255);
                v3 = _mm_div_ps(v3, m_255);
                v4 = _mm_div_ps(v4, m_255);

                // subtract 1
                v1 = _mm_sub_ps(m_1, v1);
                v2 = _mm_sub_ps(m_1, v2);
                v3 = _mm_sub_ps(m_1, v3);
                v4 = _mm_sub_ps(m_1, v4);

                // store to find min
                _mm_store_ps(tmp1, v1);
                _mm_store_ps(tmp2, v2);
                _mm_store_ps(tmp3, v3);
                _mm_store_ps(tmp4, v4);

                // find min (_mm_min_ps)
                tmp_1 = qMin(tmp1[0], qMin(tmp1[1], tmp1[2]));
                tmp_2 = qMin(tmp2[0], qMin(tmp2[1], tmp2[2]));
                tmp_3 = qMin(tmp3[0], qMin(tmp3[1], tmp3[2]));
                tmp_4 = qMin(tmp4[0], qMin(tmp4[1], tmp4[2]));

                if (!qFuzzyCompare(tmp1[3],1)) {

                    // subtract original k
                    v1 = _mm_sub_ps(v1, _mm_set_ps(tmp_1, tmp_1, tmp_1, tmp_1));

                    // subtract 1 from base k
                    float bk_min1 = 1.0 - tmp_1;

                    // divide by (cmy - k) - (1.0 - k)
                    v1 = _mm_div_ps(v1, _mm_set_ps(bk_min1, bk_min1, bk_min1, bk_min1));

                }
                if (!qFuzzyCompare(tmp2[3],1)) {

                    // subtract original k
                    v2 = _mm_sub_ps(v2, _mm_set_ps(tmp_2, tmp_2, tmp_2, tmp_2));

                    // subtract 1 from base k
                    float bk_min2 = 1.0 - tmp_2;

                    // divide by (cmy - k) - (1.0 - k)
                    v2 = _mm_div_ps(v2, _mm_set_ps(bk_min2, bk_min2, bk_min2, bk_min2));

                }
                if (!qFuzzyCompare(tmp3[3],1)) {

                    // subtract original k
                    v3 = _mm_sub_ps(v3, _mm_set_ps(tmp_3, tmp_3, tmp_3, tmp_3));

                    // subtract 1 from base k
                    float bk_min3 = 1.0 - tmp_3;

                    // divide by (cmy - k) - (1.0 - k)
                    v3 = _mm_div_ps(v3, _mm_set_ps(bk_min3, bk_min3, bk_min3, bk_min3));

                }
                if (!qFuzzyCompare(tmp4[3],1)) {

                    // subtract original k
                    v4 = _mm_sub_ps(v4, _mm_set_ps(tmp_4, tmp_4, tmp_4, tmp_4));

                    // subtract 1 from base k
                    float bk_min4 = 1.0 - tmp_4;

                    // divide by (cmy - k) - (1.0 - k)
                    v4 = _mm_div_ps(v4, _mm_set_ps(bk_min4, bk_min4, bk_min4, bk_min4));

                }

                // store to extract (prevents green cast)
                _mm_store_ps(tmp1, v1);
                _mm_store_ps(tmp2, v2);
                _mm_store_ps(tmp3, v3);
                _mm_store_ps(tmp4, v4);

                k1 = _mm_set_ps(tmp_1, tmp1[2], 0, 0); // reverse
                k2 = _mm_set_ps(tmp_2, tmp2[2], 0, 0); // reverse
                k3 = _mm_set_ps(tmp_3, tmp3[2], 0, 0); // reverse
                k4 = _mm_set_ps(tmp_4, tmp4[2], 0, 0); // reverse

                // add fourth vector back in (k)
                v1 = _mm_shuffle_ps(v1, k1, _MM_SHUFFLE(3, 2, 1, 0)); // source, dest
                v2 = _mm_shuffle_ps(v2, k2, _MM_SHUFFLE(3, 2, 1, 0)); // source, dest
                v3 = _mm_shuffle_ps(v3, k3, _MM_SHUFFLE(3, 2, 1, 0)); // source, dest
                v4 = _mm_shuffle_ps(v4, k4, _MM_SHUFFLE(3, 2, 1, 0)); // source, dest

                // multiply all by 255
                v1 = _mm_mul_ps(v1, m_255);
                v2 = _mm_mul_ps(v2, m_255);
                v3 = _mm_mul_ps(v3, m_255);
                v4 = _mm_mul_ps(v4, m_255);

                // standard store
                _mm_store_ps(tmp1, v1); // save back to tmp buffer
                _mm_store_ps(tmp2, v2); // save back to tmp buffer
                _mm_store_ps(tmp3, v3); // save back to tmp buffer
                _mm_store_ps(tmp4, v4); // save back to tmp buffer

                // mem write
                cmyk_temp[j] = tmp1[0]; // c
                cmyk_temp[j+1] = tmp1[1]; // m
                cmyk_temp[j+2] = tmp1[2]; // y
                cmyk_temp[j+3] = tmp1[3]; // k

                cmyk_temp[j+4] = tmp2[0]; // c
                cmyk_temp[j+5] = tmp2[1]; // m
                cmyk_temp[j+6] = tmp2[2]; // y
                cmyk_temp[j+7] = tmp2[3]; // k

                cmyk_temp[j+8] = tmp3[0]; // c
                cmyk_temp[j+9] = tmp3[1]; // m
                cmyk_temp[j+10] = tmp3[2]; // y
                cmyk_temp[j+11] = tmp3[3]; // k

                cmyk_temp[j+12] = tmp4[0]; // c
                cmyk_temp[j+13] = tmp4[1]; // m
                cmyk_temp[j+14] = tmp4[2]; // y
                cmyk_temp[j+15] = tmp4[3]; // k

                j+=16;

            }

        } else if(optimizationLevel == 8) {

            // assembly - SIMD Floating-Point Intrinsics
            // Hints - need to use 64-bit registeres if in 64-bit mode!
            // http://babbage.cs.qc.edu/IEEE-754/Decimal.html

            // Move data without touching caches...
            // MOVNTPS—Move Aligned Four Packed Single-FP Non Temporal

            // Could this be used for other loops (SSE 2)?
            // _mm_cvtpu8_ps - convert 4 8-bit unsigned to 4 SPFP

            // HINT: the jpeg standard only provides 3 values as input, a vector wants 4...

            // HINT: XOR source mask > destination as in: "xor $0xff, %%eax \n\t" // mask unwanted bits

            // cmp wants value, register, else we get seg-fault.

            // HINT: shufps $0xe4,%xmm1,%xmm0 (note the operands are reversed from docs!)
            // This is a general rule as psrad is the same way. The source/destination
            // flip means we must be careful with operand order.

            // HINT - grabbig low order values
            // How do you use 64-bit registers and then grab 8 bit values out of them?
            // Turns out we can use al, cl, and the other 'shadow' registers that
            // basically store the LO bits of a value, as below:
            //"CVTTSS2SI %%xmm2, %%rcx \n\t" // m
            //"add $0x2, %%r15 \n\t" // add 3 for k value insert
            //"mov %%cl, (%%rbx, %%r15, 1) \n\t"

            // data alignment - http://www.songho.ca/misc/alignment/dataalign.html
            // float* array = (float*)_aligned_malloc(SIZE*sizeof(float), 16);

            // When counting bytes, remmeber that the first byte
            // index is 0. So below, note how the index of 5 puts
            // us on the 6th element if we count from 1 -- thus, 0 index.
            // mov    %r8,(%rbx,%r15,1) r15 = 5 (magenta insert)
            // fe 00 00 00 | 00 ff 00 00 | 00 00 00 00 | 00 00 00 00

            // when moving an immediate, you need to specify the instruction size
            // eg mov %%rax, %%rcx is fine, but not mov $0x4, %%eax
            // In those cases we need to say movl movd etc.




            int t = 1;
            int l = (char)t;

            uchar *bits = imin.bits();

            int count = (wt * ht) * 4; // link with loop counter byte size

            int k_min_value;

            float * _pack_loop_array = new float[4];
            // zero out array for easier debugging
            _pack_loop_array[0] = 0;
            _pack_loop_array[1] = 0;
            _pack_loop_array[2] = 0;
            _pack_loop_array[3] = 0;

            float * _pack_lookup_table = new float[256];
            for(int i = 0; i <= 256; i++){
                _pack_lookup_table[i] = (float)i;
            }

            float * _mask_array = new float[4];
            // zero out array for easier debugging
            _mask_array[0] = 0;
            _mask_array[1] = 0;
            _mask_array[2] = 0;
            _mask_array[3] = 0;

            // mmx constants - use m128 to ensure aligned data
            __m128 m_1 = _mm_set_ps1(1.0f);
            __m128 m_255 = _mm_set_ps1(255.0f);


            // testing - clear out this array so i can see what's being inserted
            for(int i = 0; i <= count; i++){
                cmyk_temp[i] = 1;
            }

            // optimizing runs
            // base: 753 cold 712 warm

            __asm__ __volatile__(

                // set mmx constants -

                "movaps %5, %%xmm14 \n\t" // 255.0
                "movaps %4, %%xmm15 \n\t" // 1.0

                // init sign SSE sign-flip mask
                "xorps %%xmm4, %%xmm4 \n\t"

                "xor %%rax, %%rax \n\t"
                "xor %%r11, %%r11 \n\t"
                "mov $0x80000000, %%r11 \n\t"
                "mov %9, %%rbx \n\t"

                // populate mask values
                "mov %%r11,(%%rbx, %%rax, 1) \n\t"
                "add $0x4, %%rax \n\t"
                "mov %%r11,(%%rbx, %%rax, 1) \n\t"
                "add $0x4, %%rax \n\t"
                "mov %%r11,(%%rbx, %%rax, 1) \n\t"
                "add $0x4, %%rax \n\t"
                "mov %%r11,(%%rbx, %%rax, 1) \n\t"

                // populate sse reg
                "movaps (%%rbx), %%xmm4 \n\t"


                // init loop constants

                "xor %%rax, %%rax \n\t" /* init i array counter */

                "xor %%rdx, %%rdx \n\t" /* set array counter upper bounds */
                "mov %2, %%edx \n\t"

                "xor %%rcx, %%rcx \n\t" /* get base address of source array */
                "mov %1, %%rcx \n\t"

                "xor %%rbx, %%rbx \n\t" /* get base address of dest array */
                "mov %3, %%rbx \n\t"

                "xor %%r14, %%r14 \n\t" /* get base address of _pack_loopup array */
                "mov %7, %%r14 \n\t"

                "xor %%r9, %%r9 \n\t" /* L_PACK_LOOP Bounds Check ($0x10) */
                "mov $0x10, %%r9 \n\t" // check with 16 4 bytes x 4 loops

                "xor %%r10, %%r10 \n\t" /* get base address of _pack_loop_array */
                "mov %6, %%r10 \n\t"

                // init logic constants

                "xor %%r13, %%r13 \n\t"
                "mov $0x1, %%r13 \n\t" // 1 value for k_min compare

                ".L_MAIN_LOOP:" // convert each value to float, push to xmm0

                    // subroutine - created packed data from single loop

                    "xor %%r8, %%r8 \n\t" // loop counter
                    "mov $0x0, %%r8 \n\t"

                    "xor %%r11, %%r11 \n\t"
                    "xor %%r12, %%r12 \n\t" // not needed

                    "xor %%r15, %%r15 \n\t"
                    "mov %%rax, %%r15 \n\t" // PACK_INDEX




//                    ".L_PACK_LOOP:"

//                        "nop \n\t"

//                        "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

//                        "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

//                        // use lookup table to push pre-converted float val
//                        "mov (%%r14, %%r11, 4), %%r12 \n\t" // indexed value



//                        // this is slow!
//                        "mov %%r12, (%%r10, %%r8, 1) \n\t" // push to _pack_loop_array



//                        "add $0x1, %%r15 \n\t" // 1 pixel per loop iteration

//                        "add $0x4, %%r8 \n\t"

//                        "cmp %%r8, %%r9 \n\t"

//                        "jne .L_PACK_LOOP \n\t"



//                // still slow - the pushes to memory (r12->r10) slow us way down.
//                // unrolled - create 4 values inline
//                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

//                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

//                // use lookup table to push pre-converted float val
//                "mov (%%r14, %%r11, 4), %%r12 \n\t" // indexed value

//                "mov %%r12, (%%r10, %%r8, 1) \n\t" // push to _pack_loop_array

//                "add $0x1, %%r15 \n\t" // 1 pixel per loop iteration

//                "add $0x4, %%r8 \n\t"


//                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

//                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

//                // use lookup table to push pre-converted float val
//                "mov (%%r14, %%r11, 4), %%r12 \n\t" // indexed value

//                "mov %%r12, (%%r10, %%r8, 1) \n\t" // push to _pack_loop_array

//                "add $0x1, %%r15 \n\t" // 1 pixel per loop iteration

//                "add $0x4, %%r8 \n\t"


//                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

//                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

//                // use lookup table to push pre-converted float val
//                "mov (%%r14, %%r11, 4), %%r12 \n\t" // indexed value

//                "mov %%r12, (%%r10, %%r8, 1) \n\t" // push to _pack_loop_array

//                "add $0x1, %%r15 \n\t" // 1 pixel per loop iteration

//                "add $0x4, %%r8 \n\t"


//                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

//                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

//                // use lookup table to push pre-converted float val
//                "mov (%%r14, %%r11, 4), %%r12 \n\t" // indexed value

//                "mov %%r12, (%%r10, %%r8, 1) \n\t" // push to _pack_loop_array

//                "nop \n\t"
//                "nop \n\t"

//                "xor %%r8, %%r8 \n\t" // clear r8 index for push to xmm

//                "movaps (%%r10, %%r8, 1), %%xmm0 \n\t" /* move packed values into mmx */



                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"
                    "nop \n\t"



                // third attempt - mov, mask, convert, shuffle

                // c

                "add $0x2, %%r15 \n\t" // 1 pixel per loop iteration

                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

                "CVTSI2SS %%r11, %%xmm0 \n\t"

                "shufps $0xC4, %%xmm0, %%xmm0 \n\t" // 4E flip #2 and #4 11000110


                // m

                "sub $0x1, %%r15 \n\t" // 1 pixel per loop iteration

                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

                "CVTSI2SS %%r11, %%xmm0 \n\t"

                "shufps $0xE1, %%xmm0, %%xmm0 \n\t" // flip last two to get m // 11100001


                // y

                "sub $0x1, %%r15 \n\t" // 1 pixel per loop iteration

                "mov (%%rcx, %%r15, 1), %%r11 \n\t" // get source array value

                "and $0x00000000000000ff, %%r11 \n\t" // mask unwanted bits

                "CVTSI2SS %%r11, %%xmm0 \n\t"





                // perform cmy conversion

                // divide by 255
                "divps %%xmm14, %%xmm0 \n\t"

                // subtract 1
                "subps %%xmm15, %%xmm0 \n\t"


                // mask back to positive values for min processing

                // copy mask for modifier change into xmm5 (xmm4 is mask constant)
                "movaps %%xmm4, %%xmm5 \n\t"

                // copy raw value for min to xmm6
                "movaps %%xmm0, %%xmm6 \n\t"

                // create selection mask modifier
                "psrad $0x31, %%xmm6 \n\t" // for every nagative value, makes doubleword $0xffffffff

                // modify mask copy (xmm5) to mask only nagative values
                "andps %%xmm6, %%xmm5 \n\t"

                // now mask sign bits to 0 with modified mask
                "xorps %%xmm5, %%xmm0 \n\t"


                // find min value
                "movaps %%xmm0, %%xmm1 \n\t"
                "shufps $0x4E, %%xmm0, %%xmm1 \n\t" // reorder values (mask first)

                "minps %%xmm0, %%xmm1 \n\t" // find first min, put in xmm1

                "movups %%xmm1, %%xmm2 \n\t"
                "shufps $0xB1, %%xmm1, %%xmm2 \n\t"

                "minps %%xmm1, %%xmm2 \n\t" // min (k) in xmm2

                // process min logic (is min value == 1? - true = cmpeqps creates mask of all 1's)
                "movaps %%xmm15, %%xmm12 \n\t" // move 1's to xmm12
                "cmpeqps %%xmm2, %%xmm12 \n\t" // mask now in xmm12

                "movss %%xmm12, %8 \n\t" // move mask to memory value for cmp


                "cmp %8, %%r13 \n\t" // is k == 1?

                "jne .L_MULTIPLY_ALL \n\t" // anythign but 1, skip this block

                // save 1 - k value
                "movaps %%xmm15, %%xmm13 \n\t" // xmm13 is temp storage for 1.0f
                "subps %%xmm2, %%xmm13 \n\t" // 1-k in xmm13

                // subtract k from all
                "subps %%xmm2, %%xmm0 \n\t"

                // c-k / 1-k
                "divps %%xmm13, %%xmm0 \n\t"

                ".L_MULTIPLY_ALL:"

                // around 400 clocks for both!...
                "mulps %%xmm14, %%xmm0 \n\t" // cmy * 255
                "mulss %%xmm14, %%xmm2 \n\t" // k * 255


                // conversion done, export values

                "mov %%rcx, %%r11 \n\t" // save rcx so we can convert dwords to bytes using cl


                "mov %%rax, %%r15 \n\t" // pointer for insert values (r15)

                // y - truncate and export values to array
                "CVTTSS2SI %%xmm0, %%rcx \n\t" // y

                "add $0x2, %%r15 \n\t" // add 2 for y value insert
                "mov %%cl, (%%rbx, %%r15, 1) \n\t"

                // m
                "shufps $0xE1, %%xmm0, %%xmm0 \n\t" // flip last two to get m
                "CVTTSS2SI %%xmm0, %%rcx \n\t" // m

                "sub $0x1, %%r15 \n\t" // subtract 1 for m value insert
                "mov %%cl, (%%rbx, %%r15, 1) \n\t"

                // c
                "shufps $0x4E, %%xmm0, %%xmm0 \n\t" // flip #2 and #4
                "CVTTSS2SI %%xmm0, %%rcx \n\t" // c

                "mov %%cl, (%%rbx, %%rax, 1) \n\t" // rax is base address, no need to sub

                // k
                "CVTTSS2SI %%xmm2, %%rcx \n\t" // m

                "add $0x2, %%r15 \n\t" // add 3 for k value insert
                "mov %%cl, (%%rbx, %%r15, 1) \n\t"


                "mov %%r11, %%rcx \n\t" // restore rcx


                // prefetch next data line
                "prefetchT0 (%%rcx, %%r15, 1) \n\t"



                "addq $0x4, %%rax \n\t" // increment main loop counter
                "cmpq %%rdx, %%rax \n\t"

                "jne .L_MAIN_LOOP \n\t"

                : "=m" (cmyk_temp)  /* destination */
                : "m" (bits), "m" (count), "m" (cmyk_temp), "m" (m_1), "m" (m_255), "m" (_pack_loop_array), "m" (_pack_lookup_table), "m" (k_min_value), "m"(_mask_array) /* source */
                : "rax", "rbx", "rcx", "rdx", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "memory" /* clobbers */

            ); // asm

            delete _pack_lookup_table;
            delete _pack_loop_array;
            delete _mask_array;


        }// end optimization if

        //qDebug() << cmyk_temp[2];


        cmsDoTransform(hTransform,
                       cmyk_temp,
                       rgb_temp, (wt * ht));

        imout = new QImage(imin.width(), imin.height(), QImage::Format_RGB32);

        int s = 0;
        for(int h = 0; h < ht; h++){
            for(int w = 0; w < wt; w++){
                imout->setPixel(w, h, qRgb(rgb_temp[s], rgb_temp[s+1], rgb_temp[s+2]));
                s+=3;
            }
        }

        cmsDeleteTransform(hTransform);

        delete(cmyk_temp);
        delete(rgb_temp);


        if(wt > 1200 || ht > 1200){

            QImage imoutscaled = imout->scaled(QSize(1200, 1200), Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);

            imoutscaled.save(outName + ".jpg");

        } else {

            imout->save(outName + ".jpg");

        }

        }


    } // hasNext()

}
