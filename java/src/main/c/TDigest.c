
#include <jni.h>
#include "tdigest.h"
#include "com_ajwerner_tdigestc_TDigest.h"

/*
 * Class:     com_ajwerner_tdigestc_TDigest
 * Method:    td_new
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_ajwerner_tdigestc_TDigest_td_1new
(JNIEnv *env, jobject this, jint size) {
     return (jlong)(td_new((double)(size)));
}

/*
 * Class:     com_ajwerner_tdigestc_TDigest
 * Method:    td_add
 * Signature: (JDD)V
 */
JNIEXPORT void JNICALL Java_com_ajwerner_tdigestc_TDigest_td_1add
(JNIEnv *env, jobject this, jlong ptr, jdouble val, jdouble count) {
     td_add((td_histogram_t *)(ptr), (double)(val), (double)(count));
}

/*
 * Class:     com_ajwerner_tdigestc_TDigest
 * Method:    td_value_at
 * Signature: (JD)D
 */
JNIEXPORT jdouble JNICALL Java_com_ajwerner_tdigestc_TDigest_td_1value_1at
(JNIEnv *env, jobject this, jlong ptr, jdouble q) {
     
     return (jdouble)(td_value_at((td_histogram_t *)(ptr), (double)(q)));
}

/*
 * Class:     com_ajwerner_tdigestc_TDigest
 * Method:    td_free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_ajwerner_tdigestc_TDigest_td_1free
(JNIEnv *env, jobject this, jlong ptr) {
     td_free((td_histogram_t *)(ptr));
}

JNIEXPORT jdouble JNICALL Java_com_ajwerner_tdigestc_TDigest_td_1quantile_1of
(JNIEnv *env, jobject this, jlong ptr, jdouble val) {
     return (jdouble)(td_quantile_of((td_histogram_t *)(ptr), (double)(val)));
}
