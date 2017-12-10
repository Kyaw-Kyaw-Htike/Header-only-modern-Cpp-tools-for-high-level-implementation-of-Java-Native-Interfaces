#ifndef _JNI_MODERN_TOOLS_H_
#define _JNI_MODERN_TOOLS_H_

// Copyright (C) 2017 Kyaw Kyaw Htike @ Ali Abdul Ghafur. All rights reserved.

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <fstream>

/*
===================
Computing a Java function's signature
===================

It's always a set of parentheses enclosing type signifiers for the arguments, one after the other with no commas or anything, followed by a type signifier for the return value after the closing paren. It's pretty straightforward.

There's a table of type signatures on this page:

Signature    Java Type
Z    boolean
B    byte
C    char
S    short
I    int
J    long
F    float
D    double
V    void
L fully-qualified-class ;    fully-qualified-class
[ type   type[]
Those last two mean that to name a class, you say, for example, Ljava/lang/Object;, and to name an array of (for example) int, you say [I, and an array of array of int is [[I.

If you wanted to literally compute the signature in Java code based on reflection, it'd be simple enough; just use the table above with rules for handling objects and arrays.

===================
Determine the signature of a method
===================

Before calling a Java object's method from JNI, we need its signature. For example, the method
long myMethod (int n, String s, int[] arr);
is seen from JNI with the signature
(ILJAVA/LANG/STRING;[I)J
There are two parts to the signature. The first part is enclosed within the parentheses and represents the method's arguments. The second portion follows the closing parenthesis and represents the return type. The mapping between the Java type and C type is
Type     Chararacter 
boolean      Z 
byte         B 
char         C 
double       D 
float        F 
int          I 
long         J 
object       L 
short        S 
void         V 
array        [ 
Note that to specify an object, the "L" is followed by the object's class name and ends with a semi-colon, ';' .
The javap utility (included with the JDK) is very useful to show the signature to be used in JNI.

===================
JNI data type mapping in variables:
===================

Copied from header file

typedef long jint;
typedef __int64 jlong;
typedef signed char jbyte;
typedef unsigned char   jboolean;
typedef unsigned short  jchar;
typedef short           jshort;
typedef float           jfloat;
typedef double          jdouble;

*/

template<class T> struct jArrayType_to_jType;
template<> struct jArrayType_to_jType<jintArray> { typedef jint type; };
template<> struct jArrayType_to_jType<jfloatArray> { typedef jfloat type; };
template<> struct jArrayType_to_jType<jdoubleArray> { typedef jdouble type; };
template<> struct jArrayType_to_jType<jshortArray> { typedef jshort type; };
template<> struct jArrayType_to_jType<jcharArray> { typedef jchar type; };
template<> struct jArrayType_to_jType<jlongArray> { typedef jlong type; };
template<> struct jArrayType_to_jType<jbyteArray> { typedef jbyte type; };

/*
Note: can use the above as follows:

template<T>
void func()
{
	T var1;
	typedef typename jArrayType_to_jType<T>::type T2;
	T2 var2;
	typename jArrayType_to_jType<T>::type var3;
}

For func, when called as "func<jdoubleArray>", then
var1 is of type jdoubleArray, var2 and var3 are of type jdouble.

*/

// Template traits for use in the later class

template<class T> std::string get_signature_jtype(std::string s) { return s; };
template<> std::string get_signature_jtype<jint>(std::string s) { return "I"; };
template<> std::string get_signature_jtype<jfloat>(std::string s) { return "F"; };
template<> std::string get_signature_jtype<jdouble>(std::string s) { return "D"; };
template<> std::string get_signature_jtype<jshort>(std::string s) { return "S"; };
template<> std::string get_signature_jtype<jchar>(std::string s) { return "C"; };
template<> std::string get_signature_jtype<jlong>(std::string s) { return "J"; };
template<> std::string get_signature_jtype<jbyte>(std::string s) { return "B"; };
template<> std::string get_signature_jtype<jboolean>(std::string s) { return "Z"; };
template<> std::string get_signature_jtype<jstring>(std::string s) { return "Ljava/lang/String;"; };
template<> std::string get_signature_jtype<jintArray>(std::string s) { return "[I"; };
template<> std::string get_signature_jtype<jdoubleArray>(std::string s) { return "[D"; };
template<> std::string get_signature_jtype<jfloatArray>(std::string s) { return "[F"; };
template<> std::string get_signature_jtype<jshortArray>(std::string s) { return "[S"; };
template<> std::string get_signature_jtype<jcharArray>(std::string s) { return "[C"; };
template<> std::string get_signature_jtype<jlongArray>(std::string s) { return "[J"; };
template<> std::string get_signature_jtype<jbyteArray>(std::string s) { return "[B"; };
template<> std::string get_signature_jtype<void>(std::string s) { return "V"; };
template<> std::string get_signature_jtype<jobject>(std::string s) { return s; };


template<class T> struct GetFieldFunctor { T operator()(JNIEnv* env, jobject a, jfieldID b) { return 0; } };
template<> struct GetFieldFunctor<jint> { jint operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetIntField(a, b); } };
template<> struct GetFieldFunctor<jfloat> { jfloat operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetFloatField(a, b); } };
template<> struct GetFieldFunctor<jdouble> { jdouble operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetDoubleField(a, b); } };
template<> struct GetFieldFunctor<jshort> { jshort operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetShortField(a, b); } };
template<> struct GetFieldFunctor<jchar> { jchar operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetCharField(a, b); } };
template<> struct GetFieldFunctor<jlong> { jlong operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetLongField(a, b); } };
template<> struct GetFieldFunctor<jbyte> { jbyte operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetByteField(a, b); } };
template<> struct GetFieldFunctor<jboolean> { jboolean operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetBooleanField(a, b); } };
template<> struct GetFieldFunctor<jstring> { jstring operator()(JNIEnv* env, jobject a, jfieldID b) { return (jstring)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jintArray> { jintArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jintArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jdoubleArray> { jdoubleArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jdoubleArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jfloatArray> { jfloatArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jfloatArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jshortArray> { jshortArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jshortArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jcharArray> { jcharArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jcharArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jlongArray> { jlongArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jlongArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jbyteArray> { jbyteArray operator()(JNIEnv* env, jobject a, jfieldID b) { return (jbyteArray)env->GetObjectField(a, b); } };
template<> struct GetFieldFunctor<jobject> { jobject operator()(JNIEnv* env, jobject a, jfieldID b) { return env->GetObjectField(a, b); } };

template<class T> struct GetStaticFieldFunctor { T operator()(JNIEnv* env, jclass a, jfieldID b) { return 0; } };
template<> struct GetStaticFieldFunctor<jint> { jint operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticIntField(a, b); } };
template<> struct GetStaticFieldFunctor<jfloat> { jfloat operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticFloatField(a, b); } };
template<> struct GetStaticFieldFunctor<jdouble> { jdouble operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticDoubleField(a, b); } };
template<> struct GetStaticFieldFunctor<jshort> { jshort operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticShortField(a, b); } };
template<> struct GetStaticFieldFunctor<jchar> { jchar operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticCharField(a, b); } };
template<> struct GetStaticFieldFunctor<jlong> { jlong operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticLongField(a, b); } };
template<> struct GetStaticFieldFunctor<jbyte> { jbyte operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticByteField(a, b); } };
template<> struct GetStaticFieldFunctor<jboolean> { jboolean operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticBooleanField(a, b); } };
template<> struct GetStaticFieldFunctor<jstring> { jstring operator()(JNIEnv* env, jclass a, jfieldID b) { return (jstring)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jintArray> { jintArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jintArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jdoubleArray> { jdoubleArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jdoubleArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jfloatArray> { jfloatArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jfloatArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jshortArray> { jshortArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jshortArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jcharArray> { jcharArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jcharArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jlongArray> { jlongArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jlongArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jbyteArray> { jbyteArray operator()(JNIEnv* env, jclass a, jfieldID b) { return (jbyteArray)env->GetStaticObjectField(a, b); } };
template<> struct GetStaticFieldFunctor<jobject> { jobject operator()(JNIEnv* env, jclass a, jfieldID b) { return env->GetStaticObjectField(a, b); } };

template<class T> struct SetFieldFunctor { void operator()(JNIEnv* env, jobject a, jfieldID b, T c) {} };
template<> struct SetFieldFunctor<jint> { void operator()(JNIEnv* env, jobject a, jfieldID b, jint c) { env->SetIntField(a, b, c); } };
template<> struct SetFieldFunctor<jfloat> { void operator()(JNIEnv* env, jobject a, jfieldID b, jfloat c) { env->SetFloatField(a, b, c); } };
template<> struct SetFieldFunctor<jdouble> { void operator()(JNIEnv* env, jobject a, jfieldID b, jdouble c) { env->SetDoubleField(a, b, c); } };
template<> struct SetFieldFunctor<jshort> { void operator()(JNIEnv* env, jobject a, jfieldID b, jshort c) { env->SetShortField(a, b, c); } };
template<> struct SetFieldFunctor<jchar> { void operator()(JNIEnv* env, jobject a, jfieldID b, jchar c) { env->SetCharField(a, b, c); } };
template<> struct SetFieldFunctor<jlong> { void operator()(JNIEnv* env, jobject a, jfieldID b, jlong c) { env->SetLongField(a, b, c); } };
template<> struct SetFieldFunctor<jbyte> { void operator()(JNIEnv* env, jobject a, jfieldID b, jbyte c) { env->SetByteField(a, b, c); } };
template<> struct SetFieldFunctor<jboolean> { void operator()(JNIEnv* env, jobject a, jfieldID b, jboolean c) { env->SetBooleanField(a, b, c); } };
template<> struct SetFieldFunctor<jstring> { void operator()(JNIEnv* env, jobject a, jfieldID b, jstring c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jintArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jintArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jdoubleArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jdoubleArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jfloatArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jfloatArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jshortArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jshortArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jcharArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jcharArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jlongArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jlongArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jbyteArray> { void operator()(JNIEnv* env, jobject a, jfieldID b, jbyteArray c) { env->SetObjectField(a, b, (jobject)c); } };
template<> struct SetFieldFunctor<jobject> { void operator()(JNIEnv* env, jobject a, jfieldID b, jobject c) { env->SetObjectField(a, b, c); } };

template<class T> struct SetStaticFieldFunctor { void operator()(JNIEnv* env, jclass a, jfieldID b, T c) {} };
template<> struct SetStaticFieldFunctor<jint> { void operator()(JNIEnv* env, jclass a, jfieldID b, jint c) { env->SetStaticIntField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jfloat> { void operator()(JNIEnv* env, jclass a, jfieldID b, jfloat c) { env->SetStaticFloatField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jdouble> { void operator()(JNIEnv* env, jclass a, jfieldID b, jdouble c) { env->SetStaticDoubleField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jshort> { void operator()(JNIEnv* env, jclass a, jfieldID b, jshort c) { env->SetStaticShortField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jchar> { void operator()(JNIEnv* env, jclass a, jfieldID b, jchar c) { env->SetStaticCharField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jlong> { void operator()(JNIEnv* env, jclass a, jfieldID b, jlong c) { env->SetStaticLongField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jbyte> { void operator()(JNIEnv* env, jclass a, jfieldID b, jbyte c) { env->SetStaticByteField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jboolean> { void operator()(JNIEnv* env, jclass a, jfieldID b, jboolean c) { env->SetStaticBooleanField(a, b, c); } };
template<> struct SetStaticFieldFunctor<jstring> { void operator()(JNIEnv* env, jclass a, jfieldID b, jstring c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jintArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jintArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jdoubleArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jdoubleArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jfloatArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jfloatArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jshortArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jshortArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jcharArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jcharArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jlongArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jlongArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jbyteArray> { void operator()(JNIEnv* env, jclass a, jfieldID b, jbyteArray c) { env->SetStaticObjectField(a, b, (jobject)c); } };
template<> struct SetStaticFieldFunctor<jobject> { void operator()(JNIEnv* env, jclass a, jfieldID b, jobject c) { env->SetStaticObjectField(a, b, c); } };

template<class T, class... Ts> struct callXMethodFunctor { void operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) {} };
template<class... Ts> struct callXMethodFunctor<jint,Ts...> { jint operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallIntMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jfloat, Ts...> { jfloat operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallFloatMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jdouble, Ts...> { jdouble operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallDoubleMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jshort, Ts...> { jshort operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallShortMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jchar, Ts...> { jchar operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallCharMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jlong, Ts...> { jlong operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallLongMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jbyte, Ts...> { jbyte operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallByteMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jboolean, Ts...> { jboolean operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallBooleanMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jstring, Ts...> { jstring operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jstring)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jintArray, Ts...> { jintArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jintArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jdoubleArray, Ts...> { jdoubleArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jdoubleArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jfloatArray, Ts...> { jfloatArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jfloatArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jshortArray, Ts...> { jshortArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jshortArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jcharArray, Ts...> { jcharArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jcharArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jlongArray, Ts...> { jlongArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jlongArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jbyteArray, Ts...> { jbyteArray operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return (jbyteArray)env->CallObjectMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<void, Ts...> { void operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { env->CallVoidMethod(jobj, methodID, args...); } };
template<class... Ts> struct callXMethodFunctor<jobject, Ts...> { jobject operator()(JNIEnv* env, jobject jobj, jmethodID methodID, Ts... args) { return env->CallObjectMethod(jobj, methodID, args...); } };

template<class T, class... Ts> struct callXStaticMethodFunctor { void operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) {} };
template<class... Ts> struct callXStaticMethodFunctor<jint, Ts...> { jint operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticIntMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jfloat, Ts...> { jfloat operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticFloatMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jdouble, Ts...> { jdouble operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticDoubleMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jshort, Ts...> { jshort operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticShortMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jchar, Ts...> { jchar operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticCharMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jlong, Ts...> { jlong operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticLongMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jbyte, Ts...> { jbyte operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticByteMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jboolean, Ts...> { jboolean operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticBooleanMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jstring, Ts...> { jstring operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jstring)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jintArray, Ts...> { jintArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jintArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jdoubleArray, Ts...> { jdoubleArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jdoubleArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jfloatArray, Ts...> { jfloatArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jfloatArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jshortArray, Ts...> { jshortArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jshortArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jcharArray, Ts...> { jcharArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jcharArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jlongArray, Ts...> { jlongArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jlongArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jbyteArray, Ts...> { jbyteArray operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return (jbyteArray)env->CallStaticObjectMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<void, Ts...> { void operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { env->CallStaticVoidMethod(jcls, methodID, args...); } };
template<class... Ts> struct callXStaticMethodFunctor<jobject, Ts...> { jobject operator()(JNIEnv* env, jclass jcls, jmethodID methodID, Ts... args) { return env->CallStaticObjectMethod(jcls, methodID, args...); } };


// class of utility functions for dealing with JNI
class jni_utils
{
public:
	jni_utils() = delete;
	
	jni_utils(JNIEnv* env_)
	{
		env = env_;
	}

	std::string from_jstring(jstring jStr)
	{
		if (!jStr)
			return "";

		std::vector<char> charsCode;
		const jchar *chars = env->GetStringChars(jStr, nullptr);
		jsize len = env->GetStringLength(jStr);
		jsize i;

		for (i = 0; i < len; i++) {
			int code = (int)chars[i];
			charsCode.push_back(code);
		}
		env->ReleaseStringChars(jStr, chars);
		return std::string(charsCode.begin(), charsCode.end());
	}
	
	static bool from_jboolean(jboolean val)
	{
		return val == JNI_TRUE;
	}
	
	static int from_jint(jint val)
	{
		return (long)val;
	}
	
	static float from_jfloat(jfloat val)
	{
		return (float)val;
	}
	
	static double from_jdouble(jfloat val)
	{
		return (double)val;
	}
	
	std::vector<std::string> from_jstringArray(jobjectArray strArr)
	{
		int n = env->GetArrayLength(strArr);
		std::vector<std::string> strVec(n);
		jstring s;
		for (size_t i = 0; i < n; i++)
		{
			s = (jstring)(env->GetObjectArrayElement(strArr, i));
			strVec.push_back(from_jstring(s));			
		}
		return strVec;
	}

	std::vector<int> from_jintArray(jintArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jint *body = env->GetIntArrayElements(arr, 0);
		std::vector<int> arr_out(body, body + len);
		env->ReleaseIntArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<float> from_jfloatArray(jfloatArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jfloat *body = env->GetFloatArrayElements(arr, 0);
		std::vector<float> arr_out(body, body + len);
		env->ReleaseFloatArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<double> from_jdoubleArray(jdoubleArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jdouble *body = env->GetDoubleArrayElements(arr, 0);
		std::vector<double> arr_out(body, body + len);
		env->ReleaseDoubleArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<short> from_jshortArray(jshortArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jshort *body = env->GetShortArrayElements(arr, 0);
		std::vector<short> arr_out(body, body + len);
		env->ReleaseShortArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<unsigned short> from_jcharArray(jcharArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jchar *body = env->GetCharArrayElements(arr, 0);
		std::vector<unsigned short> arr_out(body, body + len);
		env->ReleaseCharArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<_int64> from_jlongArray(jlongArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jlong *body = env->GetLongArrayElements(arr, 0);
		std::vector<_int64> arr_out(body, body + len);
		env->ReleaseLongArrayElements(arr, body, 0);
		return arr_out;
	}

	std::vector<signed char> from_jbyteArray(jbyteArray arr)
	{
		jsize len = env->GetArrayLength(arr);
		jbyte *body = env->GetByteArrayElements(arr, 0);
		std::vector<signed char> arr_out(body, body + len);
		env->ReleaseByteArrayElements(arr, body, 0);
		return arr_out;
	}
	
	jstring to_jstring(std::string s)
	{
		if (s.empty())
			return nullptr;

		std::vector<char> charsCode(s.begin(), s.end());
		jchar* chars = new jchar[s.size()];
		for (size_t i = 0; i < s.size(); i++)
			chars[i] = charsCode[i];
		jstring s_out = env->NewString(chars, s.size());
		delete[] chars;
		return s_out;		
	}

	jintArray to_jintArray(std::vector<int> v)
	{
		//jintArray arr_out = env->NewIntArray(v.size());
		//std::vector<jint> buf(v.begin(), v.end());
		//env->SetIntArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jintArray arr_out = env->NewIntArray(v.size());
		jint *out_ptr = env->GetIntArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jint>(v[i]);
		env->ReleaseIntArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jfloatArray to_jfloatArray(std::vector<float> v)
	{
		//jfloatArray arr_out = env->NewFloatArray(v.size());
		//std::vector<jfloat> buf(v.begin(), v.end());
		//env->SetFloatArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jfloatArray arr_out = env->NewFloatArray(v.size());
		jfloat *out_ptr = env->GetFloatArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jfloat>(v[i]);
		std::copy(v.begin(), v.end(), out_ptr);
		env->ReleaseFloatArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jdoubleArray to_jdoubleArray(std::vector<double> v)
	{
		//jdoubleArray arr_out = env->NewDoubleArray(v.size());
		//std::vector<jdouble> buf(v.begin(), v.end());
		//env->SetDoubleArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jdoubleArray arr_out = env->NewDoubleArray(v.size());
		jdouble *out_ptr = env->GetDoubleArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jdouble>(v[i]);
		std::copy(v.begin(), v.end(), out_ptr);
		env->ReleaseDoubleArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jshortArray to_jshortArray(std::vector<short> v)
	{
		//jshortArray arr_out = env->NewShortArray(v.size());
		//std::vector<jshort> buf(v.begin(), v.end());
		//env->SetShortArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jshortArray arr_out = env->NewShortArray(v.size());
		jshort *out_ptr = env->GetShortArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jshort>(v[i]);
		std::copy(v.begin(), v.end(), out_ptr);
		env->ReleaseShortArrayElements(arr_out, out_ptr, 0);
		return arr_out;

	}

	jcharArray to_jcharArray(std::vector<unsigned short> v)
	{
		//jcharArray arr_out = env->NewCharArray(v.size());
		//std::vector<jchar> buf(v.begin(), v.end());
		//env->SetCharArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jcharArray arr_out = env->NewCharArray(v.size());
		jchar *out_ptr = env->GetCharArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jchar>(v[i]);
		env->ReleaseCharArrayElements(arr_out, out_ptr, 0);
		return arr_out;

	}

	jlongArray to_jlongArray(std::vector<_int64> v)
	{
		//jlongArray arr_out = env->NewLongArray(v.size());
		//std::vector<jlong> buf(v.begin(), v.end());
		//env->SetLongArrayRegion(arr_out, 0, v.size(), buf.data());
		//return arr_out;

		jlongArray arr_out = env->NewLongArray(v.size());
		jlong *out_ptr = env->GetLongArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jlong>(v[i]);
		env->ReleaseLongArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jbyteArray to_jbyteArray(std::vector<signed char> v)
	{
		//jbyteArray arr_out = env->NewByteArray(v.size());
		//std::vector<jbyte> buf(v.begin(), v.end());
		//env->SetByteArrayRegion(arr_out, 0, v.size(), buf.data());

		jbyteArray arr_out = env->NewByteArray(v.size());
		jbyte *out_ptr = env->GetByteArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jbyte>(v[i]);
		env->ReleaseByteArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}
	
	jintArray to_jintArray(int* v, int size_v)
	{
		jintArray arr_out = env->NewIntArray(size_v);
		jint *out_ptr = env->GetIntArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jint>(v[i]);
		env->ReleaseIntArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jfloatArray to_jfloatArray(float* v, int size_v)
	{
		jfloatArray arr_out = env->NewFloatArray(size_v);
		jfloat *out_ptr = env->GetFloatArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jfloat>(v[i]);
		std::copy(v, v+size_v, out_ptr);
		env->ReleaseFloatArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jdoubleArray to_jdoubleArray(double* v, int size_v)
	{
		jdoubleArray arr_out = env->NewDoubleArray(size_v);
		jdouble *out_ptr = env->GetDoubleArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jdouble>(v[i]);
		std::copy(v, v+size_v, out_ptr);
		env->ReleaseDoubleArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jshortArray to_jshortArray(short* v, int size_v)
	{
		jshortArray arr_out = env->NewShortArray(size_v);
		jshort *out_ptr = env->GetShortArrayElements(arr_out, 0);
		// for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			// out_ptr[i] = static_cast<jshort>(v[i]);
		std::copy(v, v+size_v, out_ptr);
		env->ReleaseShortArrayElements(arr_out, out_ptr, 0);
		return arr_out;

	}

	jcharArray to_jcharArray(unsigned short* v, int size_v)
	{
		jcharArray arr_out = env->NewCharArray(size_v);
		jchar *out_ptr = env->GetCharArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jchar>(v[i]);
		env->ReleaseCharArrayElements(arr_out, out_ptr, 0);
		return arr_out;

	}

	jlongArray to_jlongArray(_int64* v, int size_v)
	{
		jlongArray arr_out = env->NewLongArray(size_v);
		jlong *out_ptr = env->GetLongArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jlong>(v[i]);
		env->ReleaseLongArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}

	jbyteArray to_jbyteArray(signed char* v, int size_v)
	{
		jbyteArray arr_out = env->NewByteArray(size_v);
		jbyte *out_ptr = env->GetByteArrayElements(arr_out, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr_out); i++)
			out_ptr[i] = static_cast<jbyte>(v[i]);
		env->ReleaseByteArrayElements(arr_out, out_ptr, 0);
		return arr_out;
	}	
	
	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jintArray_inputArg(std::vector<int> v, jintArray arr)
	{
		jint *arr_ptr = env->GetIntArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jint>(v[i]);
		env->ReleaseIntArrayElements(arr, arr_ptr, 0);
	}
	
	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jfloatArray_inputArg(std::vector<float> v, jfloatArray arr)
	{
		jfloat *arr_ptr = env->GetFloatArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jfloat>(v[i]);
		env->ReleaseFloatArrayElements(arr, arr_ptr, 0);
	}

	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jdoubleArray_inputArg(std::vector<double> v, jdoubleArray arr)
	{
		jdouble *arr_ptr = env->GetDoubleArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jdouble>(v[i]);
		env->ReleaseDoubleArrayElements(arr, arr_ptr, 0);
	}

	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jshortArray_inputArg(std::vector<short> v, jshortArray arr)
	{
		jshort *arr_ptr = env->GetShortArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jshort>(v[i]);
		env->ReleaseShortArrayElements(arr, arr_ptr, 0);
	}

	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jlongArray_inputArg(std::vector<_int64> v, jlongArray arr)
	{
		jlong *arr_ptr = env->GetLongArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jlong>(v[i]);
		env->ReleaseLongArrayElements(arr, arr_ptr, 0);
	}

	// directly change/set values of array input argument
	// which has been allocated in java with the values from C++ vector.
	void set_jbyteArray_inputArg(std::vector<signed char> v, jbyteArray arr)
	{
		jbyte *arr_ptr = env->GetByteArrayElements(arr, 0);
		for (size_t i = 0; i < env->GetArrayLength(arr); i++)
			arr_ptr[i] = static_cast<jbyte>(v[i]);
		env->ReleaseByteArrayElements(arr, arr_ptr, 0);
	}
	
	void throw_exception(std::string msg)
	{
		jclass cls = env->FindClass("java/lang/IllegalArgumentException");
		env->ThrowNew(cls, msg.c_str());
	}
	
	// get the fully qualified class name signature by reflection
	// template param T should be jobject. If any other type such as
	// jint and jfloatArray, just return empty string.
	template<class T>
	std::string get_signature_jobject(T obj, bool just_class_name = false)
	{	
		if (!std::is_same<T, jobject>::value) return "";
		jobject clsObj = env->CallObjectMethod(obj, env->GetMethodID(env->GetObjectClass(obj), "getClass", "()Ljava/lang/Class;"));
		jstring className_temp = (jstring)env->CallObjectMethod(clsObj, env->GetMethodID(env->GetObjectClass(clsObj), "getName", "()Ljava/lang/String;"));
		std::string str_classname = from_jstring(className_temp);
		std::replace(str_classname.begin(), str_classname.end(), '.', '/');
		std::string str_class_sig = "L" + str_classname + ";";
		if(just_class_name)
			return str_classname;
		return str_class_sig;			
	}
	
	// generate a complete signature string for a java method
	// the first template parameter is the return type of the method. The rest
	// is the parameter types for the method input arguments.
	// This function will automatically generate the signature for the entire
	// method. Except for the output return type of jobject. In this case,
	// I need to give its signature (e.g. "LKKH/StdLib/Matkc") by setting the 
	// the input string sig_return_type_if_type_returnVal_is_jobject.
	// Otherwise, can just set it to any garbage value such as "".
	template<typename type_returnVal, typename... types_inputArgs>
	std::string get_signature_jmethod(std::string sig_return_type_if_type_returnVal_is_jobject, types_inputArgs... inputArgs)
	{
		// opening (will now put input argument types' signatures)
		std::string sig_method = "(";
		// input arguments
		sig_method += get_signature_jmethod_inputArgs<types_inputArgs...>(inputArgs...);
		// close
		sig_method += ")";
		// now add to the string the return type signature
		sig_method += get_signature_jtype<type_returnVal>
		(sig_return_type_if_type_returnVal_is_jobject);
		return sig_method;
	}
		
	// Note, the method below is not really for usage. It is just for recording purposes
	// of JNI usage. I can use the JavaClass class to conveniently manipulate
	// java objects including calling methods, setting and getting fields.
	// call method of an object (jobject) or static method of a class (jclass).
	// The method is identified by the 
	// method name and the method signature
	// After the method signature, actual method arguments follow.
	// template param T is the return argument of the method and can be 
	// jint, jfloat, ..., jintArray, ..., jobject
	// template param jobject_or_jclass should be only either jobject or jclass
	// depending whether the method to be called is non-static or static method respectively
	template<class T, class jobject_or_jclass>
	T call_method_general(jobject_or_jclass obj_or_cls, std::string methodname, std::string method_sig, ...)
	{
		jmethodID mID;

		if (std::is_same<jobject_or_jclass, jobject>::value) // non-static methods
		{
			mID = env->GetMethodID(env->GetObjectClass(obj_or_class), methodname.c_str(), method_sig.c_str());
			if (std::is_same<T, jobject>::value)
				return env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jint>::value)
				return env->CallIntMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jfloat>::value)
				return env->CallFloatMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jdouble>::value)
				return env->CallDoubleMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jshort>::value)
				return env->CallShortMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jchar>::value)
				return env->CallCharMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jlong>::value)
				return env->CallLongMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jbyte>::value)
				return env->CallByteMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jboolean>::value)
				return env->CallBooleanMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jstring>::value)
				return env->CallStringMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jintArray>::value)
				return (jintArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jdoubleArray>::value)
				return (jdoubleArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jfloatArray>::value)
				return (jfloatArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jshortArray>::value)
				return (jshortArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jcharArray>::value)
				return (jcharArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jlongArray>::value)
				return (jlongArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jbyteArray>::value)
				return (jbyteArray)env->CallObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, void>::value)
				env->CallVoidMethod(obj_or_class, mID, ...);
		}
		else // static methods
		{
			mID = env->GetStaticMethodID(obj_or_class, methodname.c_str(), method_sig.c_str());
			if (std::is_same<T, jobject>::value)
				return env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jint>::value)
				return env->CallStaticIntMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jfloat>::value)
				return env->CallStaticFloatMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jdouble>::value)
				return env->CallStaticDoubleMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jshort>::value)
				return env->CallStaticShortMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jchar>::value)
				return env->CallStaticCharMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jlong>::value)
				return env->CallStaticLongMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jbyte>::value)
				return env->CallStaticByteMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jboolean>::value)
				return env->CallStaticBooleanMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jstring>::value)
				return env->CallStaticStringMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jintArray>::value)
				return (jintArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jdoubleArray>::value)
				return (jdoubleArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jfloatArray>::value)
				return (jfloatArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jshortArray>::value)
				return (jshortArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jcharArray>::value)
				return (jcharArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jlongArray>::value)
				return (jlongArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, jbyteArray>::value)
				return (jbyteArray)env->CallStaticObjectMethod(obj_or_class, mID, ...);
			else if (std::is_same<T, void>::value)
				env->CallStaticVoidMethod(obj_or_class, mID, ...);
		}
	}


private:

	JNIEnv* env;
	
	// for input arguments only (call itself recursively)
	// this is a helper function for get_signature_jmethod
	template<typename T, typename... Ts>
	std::string get_signature_jmethod_inputArgs(T arg, Ts... args)
	{		
		return get_signature_jtype<T>(get_signature_jobject(arg)) + get_signature_jmethod_inputArgs<Ts...>(args...);
	}
	
	// the stopping point for the recursion
	// this is a helper function for get_signature_jmethod
	template<typename T>
	std::string get_signature_jmethod_inputArgs(T arg)
	{
		return get_signature_jtype<T>(get_signature_jobject(arg));
	}

};


// wrapper class for Matkc Java matrix class
class Matkc
{
private:

	JNIEnv* env;

	jclass cls;	
	jobject obj;

	jmethodID constructor_methodID;

	jfieldID fieldID_data;	
	jfieldID fieldID_nr;
	jfieldID fieldID_nc;
	jfieldID fieldID_nch;
	jfieldID fieldID_ndata_per_chan;
	jfieldID fieldID_ndata;

	jdoubleArray data;
	jdouble* ptr_data;

	jint nr, nc, nch, nd, ndpch;

	void prep_class_info(JNIEnv* env_)
	{
		env = env_;
		cls = env->FindClass("KKH/StdLib/Matkc");
		constructor_methodID = env->GetMethodID(cls, "<init>", "(III)V");
		fieldID_data = env->GetFieldID(cls, "data", "[D");
		fieldID_nr = env->GetFieldID(cls, "nr", "I");
		fieldID_nc = env->GetFieldID(cls, "nc", "I");
		fieldID_nch = env->GetFieldID(cls, "nch", "I");
		fieldID_ndata = env->GetFieldID(cls, "ndata", "I");
		fieldID_ndata_per_chan = env->GetFieldID(cls, "ndata_per_chan", "I");
	}

	void create_new_Java_matrix(int nrows, int ncols, int nchannels)
	{
		obj = env->NewObject(cls, constructor_methodID, nrows, ncols, nchannels);
	}

	void wrap_existing_Java_matrix(jobject obj_matJava)
	{
		obj = obj_matJava;
	}

	void prep_data_info()
	{
		data = (jdoubleArray)env->GetObjectField(obj, fieldID_data);		
		ptr_data = env->GetDoubleArrayElements(data, 0);
		nr = env->GetIntField(obj, fieldID_nr);
		nc = env->GetIntField(obj, fieldID_nc);
		nch = env->GetIntField(obj, fieldID_nch);
		ndpch = env->GetIntField(obj, fieldID_ndata_per_chan);
		nd = env->GetIntField(obj, fieldID_ndata);		
	}

	void init_new(JNIEnv* env_, int nrows, int ncols, int nchannels)
	{
		prep_class_info(env_);
		create_new_Java_matrix(nrows, ncols, nchannels);
		prep_data_info();
	}

	void init_new(JNIEnv* env_, jobject obj_Matkc)
	{
		prep_class_info(env_);
		wrap_existing_Java_matrix(obj_Matkc);
		prep_data_info();
	}

public:

	~Matkc()
	{
		env->ReleaseDoubleArrayElements(data, ptr_data, 0);
	}

	Matkc() {}

	Matkc(const Matkc &m)
	{
		init_new(m.env, m.nrows(), m.ncols(), m.nchannels());
		std::copy(m.ptr_data, m.ptr_data + nd, ptr_data);
	}

	// create a new Matkc 
	void create(JNIEnv* env_, int nrows, int ncols, int nchannels = 1)
	{
		init_new(env_, nrows, ncols, nchannels);
	}
	
	// wrap an existing Matkc from Java
	// no copy of data
	void create(JNIEnv* env_, jobject obj_Matkc)
	{
		init_new(env_, obj_Matkc);
	}

	// construct from opencv matrix (make copy of data)
	// assumes that the opencv matrix is a 2D matrix with a variable number of channels
	// template param T should be C++ native types such as float, double, int and unsigned char
	template<class T, int nchannels>
	void create(JNIEnv* env_, cv::Mat mIn, bool divBy255 = false)
	{
		int nrows = mIn.rows;
		int ncols = mIn.cols;

		jni_utils ju(env_);

		if (mIn.channels() != nchannels)
		{
			ju.throw_exception("ERROR from JNI: Input opencv matrix does not have the same number of channels as the specified template parameter.");
			return;
		}			

		if (mIn.dims != 2)
		{
			ju.throw_exception("ERROR from JNI: Input opencv matrix is not 2D (with a variable number of channels).");
			return;
		}

		init_new(env_, nrows, ncols, nchannels);

		double divBy;
		if (divBy255)
			divBy = 255;
		else
			divBy = 1;
		
		int cc = 0;
		for (int k = 0; k < nchannels; k++)
			for (int j = 0; j < ncols; j++)
				for (int i = 0; i < nrows; i++)
					ptr_data[cc++] = static_cast<double>(mIn.at<cv::Vec<T, nchannels>>(i, j)[k]) / divBy;
	}

	// construct from opencv IplImage (make copy of data)
	// assumes that the opencv matrix is a 2D matrix with a variable number of channels
	// template param T should be C++ native types such as float, double, int and unsigned char
	template<class T, int nchannels>
	void create(JNIEnv* env_, IplImage* img_ipl, bool divBy255 = false)
	{		
		cv::Mat mIn = cv::cvarrToMat(img_ipl, false);

		int nrows = mIn.rows;
		int ncols = mIn.cols;

		jni_utils ju(env_);

		if (mIn.channels() != nchannels)
		{
			ju.throw_exception("ERROR from JNI: Input opencv matrix does not have the same number of channels as the specified template parameter.");
			return;
		}

		if (mIn.dims != 2)
		{
			ju.throw_exception("ERROR from JNI: Input opencv matrix is not 2D (with a variable number of channels).");
			return;
		}

		init_new(env_, nrows, ncols, nchannels);

		double divBy;
		if (divBy255)
			divBy = 255;
		else
			divBy = 0;

		int cc = 0;
		for (int k = 0; k < nchannels; k++)
			for (int j = 0; j < ncols; j++)
				for (int i = 0; i < nrows; i++)
					ptr_data[cc++] = static_cast<double>(mIn.at<cv::Vec<T, nchannels>>(i, j)[k]) / divBy;
	}
	
	// construct from std::vector (make copy of data)
	// template parameter T should be native C++ types such as float, double, int and unsigned char
	template<class T>
	void create(JNIEnv* env_, std::vector<T> vIn, int nrows, int ncols, int nchannels, bool transpose = false)
	{
		jni_utils ju(env_);

		if (vIn.size() != nrows * ncols * nchannels)
		{
			ju.throw_exception("ERROR from JNI: vIn.size() != nrows * ncols * nchannels.");
			return;
		}
			
		init_new(env_, nrows, ncols, nchannels);

		if (!transpose) // assume the data in the the vector is stored in col major
		{
			if (std::is_same<T, double>::value)
			{
				std::copy(vIn.begin(), vIn.end(), ptr_data);
			}

			else
			{
				for (int ii = 0; ii < vIn.size(); ii++)
					ptr_data[ii] = static_cast<double>(vIn[ii]);
			}
		}
		else // assume the data in the the vector is stored in row major
		{
			unsigned long long cc = 0;
			int nch_mult_nc = nch * nc;
			for (int k = 0; k < nch; k++)
				for (int j = 0; j < nc; j++)
					for (int i = 0; i < nr; i++)
						ptr_data[cc++] = static_cast<double>(vIn[i * nch_mult_nc + j * nch + k]);
		}			
	}

	// construct from raw data pointer (make copy of data)
	// template parameter T should be native C++ types such as float, double, int and unsigned char
	template<class T>
	void create(JNIEnv* env_, T* ptr_in, int nrows, int ncols, int nchannels)
	{
		init_new(env_, nrows, ncols, nchannels);

		if (std::is_same<T, double>::value)
			std::copy(ptr_in, ptr_in + nd, ptr_data);
		else
		{
			for (int ii = 0; ii < nd; ii++)
				ptr_data[ii] = static_cast<double>(ptr_in[ii]);
		}
	}

	// type T should be native C++ types such as float, double and unsigned char
	template<class T>
	void create(JNIEnv* env_, arma::Mat<T> mIn)
	{		
		int nrows = mIn.n_rows;
		int ncols = mIn.n_cols;
		int nchannels = 1;

		init_new(env_, nrows, ncols, nchannels);

		T* ptr_in = mIn.memptr();

		if (std::is_same<T, double>::value)
			std::copy(ptr_in, ptr_in + nd, ptr_data);
		else
		{
			for (int ii = 0; ii < nd; ii++)
				ptr_data[ii] = static_cast<double>(ptr_in[ii]);
		}
	}

	// type T should be native C++ types such as float, double and unsigned char
	template<class T>
	void create(JNIEnv* env_, arma::Cube<T> mIn)
	{
		int nrows = mIn.n_rows;
		int ncols = mIn.n_cols;
		int nchannels = mIn.n_slices;

		init_new(env_, nrows, ncols, nchannels);

		T* ptr_in = mIn.memptr();

		if (std::is_same<T, double>::value)
			std::copy(ptr_in, ptr_in + nd, ptr_data);
		else
		{
			for (int ii = 0; ii < nd; ii++)
				ptr_data[ii] = static_cast<double>(ptr_in[ii]);
		}
	}
	
	// template parameter T should be native C++ types such as float, double, int and unsigned char
	template<class T>
	std::vector<T> to_stdVec(bool transpose = false)
	{
		std::vector<T> vOut(nd);

		if (!transpose)
		{
			if (std::is_same<T, double>::value)
				std::copy(ptr_data, ptr_data + nd, vOut.begin());
			else
			{
				for (int ii = 0; ii < nd; ii++)
					vOut[ii] = static_cast<T>(ptr_data[ii]);
			}
		}

		else
		{
			unsigned long long cc = 0;
			for (int i = 0; i < nr; i++)
				for (int j = 0; j < nc; j++)
					for (int k = 0; k < nch; k++)
						vOut[cc++] = get(i,j,k);
		}
		
		return vOut;
	}

	// convert to opencv matrix (make copy of data)
	// the output opencv matrix will be a 2D matrix with a variable number of channels
	// template param T should be C++ native types such as float, double, int and unsigned char
	template<class T, int nchannels>
	cv::Mat to_cvMat(bool multBy255 = false)
	{
		jni_utils ju(env);
			
		if (nchannels != nch)
		{
			ju.throw_exception("ERROR from JNI: template parameter nchannels does not match with internal matrix nchannels.");
			return cv::Mat();
		}

		double multBy;
		if (multBy255)
			multBy = 255;
		else
			multBy = 1;
			
		cv::Mat mOut(nr, nc, CV_MAKE_TYPE(cv::DataType<T>::depth, nch));
		int cc = 0;
		for (int k = 0; k < nch; k++)
			for (int j = 0; j < nc; j++)
				for (int i = 0; i < nr; i++)
					mOut.at<cv::Vec<T, nchannels>>(i, j)[k] = static_cast<T>(ptr_data[cc++] * multBy);

		return mOut;
	}

	// type T should be native C++ types such as float, double and unsigned char
	template<class T>
	arma::Cube<T> to_armaCube()
	{
		arma::Cube<T> mOut(nr, nc, nch);		
		T* ptr_out = mOut.memptr();

		if (std::is_same<T, double>::value)
			std::copy(ptr_data, ptr_data + nd, ptr_out);
		else
		{
			for (int ii = 0; ii < nd; ii++)
				ptr_out[ii] = static_cast<T>(ptr_data[ii]);
		}

		return mOut;
	}

	// type T should be native C++ types such as float, double and unsigned char
	template<class T>
	arma::Mat<T> to_armaMat()
	{
		jni_utils ju(env);

		if (nch != 1)
		{
			ju.throw_exception("ERROR from JNI: to_armMat() can only called if the underlying matrix has 1 channel.");
			return arma::Mat<T>();
		}			

		arma::Mat<T> mOut(nr, nc);
		T* ptr_out = mOut.memptr();

		if (std::is_same<T, double>::value)
			std::copy(ptr_data, ptr_data + nd, ptr_out);
		else
		{
			for (int ii = 0; ii < nd; ii++)
				ptr_out[ii] = static_cast<T>(ptr_data[ii]);
		}

		return mOut;
	}

	double get(int i, int j, int k)
	{
		return ptr_data[k * ndpch + j * nr + i];
	}

	// get the copy of data corresponding to given range of a full matrix
	Matkc get(int r1, int r2, int c1, int c2, int ch1, int ch2)
	{
		if (r1 == -1) r1 = nr - 1;
		if (r2 == -1) r2 = nr - 1;
		if (c1 == -1) c1 = nc - 1;
		if (c2 == -1) c2 = nc - 1;
		if (ch1 == -1) ch1 = nch - 1;
		if (ch2 == -1) ch2 = nch - 1;

		int nr_new = r2 - r1 + 1;
		int nc_new = c2 - c1 + 1;
		int nch_new = ch2 - ch1 + 1;
		int ndata_per_chan_new = nr_new * nc_new;
		int ndata_new = ndata_per_chan_new * nch_new;

		Matkc mOut;
		mOut.create(env, nr_new, nc_new, nch_new);
		double* temp_out = mOut.ptr_data;

		if (nr_new == nr && nc_new == nc)
		{
			std::copy(ptr_data + ch1 * ndpch, ptr_data + ch1 * ndpch + ndata_new, temp_out);
		}
		else if (nr_new == nr && nc_new != nc)
		{
			int cc = 0;
			int idx_temp;
			for (int k = 0; k<nch_new; k++)
			{
				idx_temp = (k + ch1) * ndpch + c1*nr;
				std::copy(ptr_data + idx_temp, ptr_data + idx_temp + ndata_per_chan_new, temp_out + cc);
				cc += nr_new * nc_new;
			}
		}
		else
		{
			int cc = 0;
			int idx_temp;
			for (int k = 0; k<nch_new; k++)
				for (int j = 0; j<nc_new; j++)
				{					
					idx_temp = (k + ch1) * ndpch + (j + c1) * nr + r1;
					std::copy(ptr_data + idx_temp, ptr_data + idx_temp + nr_new, temp_out + cc);
					cc += nr_new;
				}
		}

		return mOut;
	}

	// get the copy of data corresponding to given range of a full matrix
	std::vector<double> get_stdVecOutput(int r1, int r2, int c1, int c2, int ch1, int ch2)
	{
		if (r1 == -1) r1 = nr - 1;
		if (r2 == -1) r2 = nr - 1;
		if (c1 == -1) c1 = nc - 1;
		if (c2 == -1) c2 = nc - 1;
		if (ch1 == -1) ch1 = nch - 1;
		if (ch2 == -1) ch2 = nch - 1;

		int nr_new = r2 - r1 + 1;
		int nc_new = c2 - c1 + 1;
		int nch_new = ch2 - ch1 + 1;
		int ndata_per_chan_new = nr_new * nc_new;
		int ndata_new = ndata_per_chan_new * nch_new;

		Matkc mOut;
		mOut.create(env, nr_new, nc_new, nch_new);
		std::vector<double> vOut(nr_new * nc_new * nch_new);
		double* temp_out = vOut.data();

		if (nr_new == nr && nc_new == nc)
		{
			std::copy(ptr_data + ch1 * ndpch, ptr_data + ch1 * ndpch + ndata_new, temp_out);
		}
		else if (nr_new == nr && nc_new != nc)
		{
			int cc = 0;
			int idx_temp;
			for (int k = 0; k<nch_new; k++)
			{
				idx_temp = (k + ch1) * ndpch + c1*nr;
				std::copy(ptr_data + idx_temp, ptr_data + idx_temp + ndata_per_chan_new, temp_out + cc);
				cc += nr_new * nc_new;
			}
		}
		else
		{
			int cc = 0;
			int idx_temp;
			for (int k = 0; k<nch_new; k++)
				for (int j = 0; j<nc_new; j++)
				{
					idx_temp = (k + ch1) * ndpch + (j + c1) * nr + r1;
					std::copy(ptr_data + idx_temp, ptr_data + idx_temp + nr_new, temp_out + cc);
					cc += nr_new;
				}
		}

		return vOut;
	}

	Matkc get(int r1, int r2, int c1, int c2)
	{
		return get(r1, r2, c1, c2, 0, -1);
	}

	std::vector<double> get_stdVecOutput(int r1, int r2, int c1, int c2)
	{
		return get_stdVecOutput(r1, r2, c1, c2, 0, -1);
	}
	
	// assume k=0
	double get(int i, int j)
	{
		return ptr_data[j * nr + i];
	}

	// get from a linear index
	double get(int lin_index)
	{
		return ptr_data[lin_index];
	}

	// get first element
	double get()
	{
		return ptr_data[0];
	}

	// get a discontinuous submatrix of the current matrix.
	Matkc get(std::vector<int> row_indices, std::vector<int> col_indices, std::vector<int> channel_indices)
	{
		int nr_new = row_indices.size();
		int nc_new = col_indices.size();
		int nch_new = channel_indices.size();
		Matkc mOut;
		mOut.create(env, nr_new, nc_new, nch_new);
		double* temp_out = mOut.ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[cc++] = ptr_data[channel_indices[k] * ndpch + col_indices[j] * nr + row_indices[i]];

		return mOut;
	}

	// get a discontinuous submatrix of the current matrix.
	std::vector<double> get_stdVecOutput(std::vector<int> row_indices, std::vector<int> col_indices, std::vector<int> channel_indices)
	{
		int nr_new = row_indices.size();
		int nc_new = col_indices.size();
		int nch_new = channel_indices.size();
		std::vector<double> vOut(nr_new * nc_new * nch_new);
		double* temp_out = vOut.data();

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[cc++] = ptr_data[channel_indices[k] * ndpch + col_indices[j] * nr + row_indices[i]];

		return vOut;
	}

	Matkc get_row(int row_index)
	{
		return get(row_index, row_index, 0, -1, 0, -1);
	}

	std::vector<double> get_row_stdVecOutput(int row_index)
	{
		return get_stdVecOutput(row_index, row_index, 0, -1, 0, -1);
	}

	Matkc get_rows(int start_index, int end_index)
	{
		return get(start_index, end_index, 0, -1, 0, -1);
	}

	std::vector<double> get_rows_stdVecOutput(int start_index, int end_index)
	{
		return get_stdVecOutput(start_index, end_index, 0, -1, 0, -1);
	}

	// take a discontinuous submatrix in the form of rows
	Matkc get_rows(std::vector<int> row_indices)
	{
		int nr_new = row_indices.size();
		Matkc mOut;
		mOut.create(env, nr_new, nc, nch);
		double* temp_out = mOut.ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[cc++] = ptr_data[k * ndpch + j * nr + row_indices[i]];

		return mOut;
	}

	std::vector<double> get_rows_stdVecOutput(std::vector<int> row_indices)
	{
		int nr_new = row_indices.size();
		std::vector<double> temp_out(nr_new * nc * nch);

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[cc++] = ptr_data[k * ndpch + j * nr + row_indices[i]];

		return temp_out;
	}

	// take a continuous submatrix in the form of a column
	Matkc get_col(int col_index)
	{
		return get(0, -1, col_index, col_index, 0, -1);
	}

	std::vector<double> get_col_stdVecOutput(int col_index)
	{
		return get_stdVecOutput(0, -1, col_index, col_index, 0, -1);
	}

	// take a continuous submatrix in the form of cols
	Matkc get_cols(int start_index, int end_index)
	{
		return get(0, -1, start_index, end_index, 0, -1);
	}

	std::vector<double> get_cols_stdVecOutput(int start_index, int end_index)
	{
		return get_stdVecOutput(0, -1, start_index, end_index, 0, -1);
	}

	// take a discontinuous submatrix in the form of cols
	Matkc get_cols(std::vector<int> col_indices)
	{
		int nc_new = col_indices.size();
		Matkc mOut;
		mOut.create(env, nr, nc_new, nch);
		double* temp_out = mOut.ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr; i++)
					temp_out[cc++] = ptr_data[k * ndpch + col_indices[j] * nr + i];

		return mOut;
	}

	std::vector<double> get_cols_stdVecOutput(std::vector<int> col_indices)
	{
		int nc_new = col_indices.size();
		std::vector<double> temp_out(nr * nc_new * nch);

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr; i++)
					temp_out[cc++] = ptr_data[k * ndpch + col_indices[j] * nr + i];

		return temp_out;
	}

	// take a continuous submatrix in the form of a channel
	Matkc get_channel(int channel_index)
	{
		return get(0, -1, 0, -1, channel_index, channel_index);
	}

	std::vector<double> get_channel_stdVecOutput(int channel_index)
	{
		return get_stdVecOutput(0, -1, 0, -1, channel_index, channel_index);
	}

	// take a continuous submatrix in the form of channels
	Matkc get_channels(int start_index, int end_index)
	{
		return get(0, -1, 0, -1, start_index, end_index);
	}

	std::vector<double> get_channels_stdVecOutput(int start_index, int end_index)
	{
		return get_stdVecOutput(0, -1, 0, -1, start_index, end_index);
	}

	// take a discontinuous submatrix in the form of cols
	Matkc get_channels(std::vector<int> channel_indices)
	{
		int nch_new = channel_indices.size();
		Matkc mOut;
		mOut.create(env, nr, nc, nch_new);
		double* temp_out = mOut.ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr; i++)
					temp_out[cc++] = ptr_data[channel_indices[k] * ndpch + j * nr + i];

		return mOut;
	}

	std::vector<double> get_channels_stdVecOutput(std::vector<int> channel_indices)
	{
		int nch_new = channel_indices.size();
		std::vector<double> temp_out(nr * nc * nch_new);

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr; i++)
					temp_out[cc++] = ptr_data[channel_indices[k] * ndpch + j * nr + i];

		return temp_out;
	}
	
	// use the entire given input matrix to set part of this matrix with the given range
	void set(Matkc mIn, int r1, int r2, int c1, int c2, int ch1, int ch2)
	{
		if (r1 == -1) r1 = nr - 1;
		if (r2 == -1) r2 = nr - 1;
		if (c1 == -1) c1 = nc - 1;
		if (c2 == -1) c2 = nc - 1;
		if (ch1 == -1) ch1 = nch - 1;
		if (ch2 == -1) ch2 = nch - 1;

		int nr_new = r2 - r1 + 1;
		int nc_new = c2 - c1 + 1;
		int nch_new = ch2 - ch1 + 1;
		int ndata_per_chan_new = nr_new * nc_new;
		int ndata_new = ndata_per_chan_new * nch_new;

		if (nr_new != mIn.nr || nc_new != mIn.nc || nch_new != mIn.nch)
		{
			jni_utils ju(env);
			ju.throw_exception("ERROR from JNI: the input matrix and the range specified do not match.");
			return;
		}			

		double* temp_in = mIn.ptr_data;
		double* temp_out = ptr_data;

		if (nr_new == nr && nc_new == nc)
		{
			std::copy(temp_in, temp_in + ndata_new, temp_out + ch1 * ndpch);
		}
		else if (nr_new == nr && nc_new != nc)
		{
			int cc = 0;
			for (int k = 0; k<nch_new; k++)
			{
				std::copy(temp_in + cc, temp_in + cc + ndata_per_chan_new, temp_out + (k + ch1) * ndpch + c1*nr);
				cc += nr_new * nc_new;
			}
		}
		else
		{
			int cc = 0;
			for (int k = 0; k<nch_new; k++)
				for (int j = 0; j<nc_new; j++)
				{
					std::copy(temp_in + cc, temp_in + cc + nr_new, temp_out + (k + ch1) * ndpch + (j + c1) * nr + r1);
					cc += nr_new;
				}
		}

	}

	// use the entire given input matrix (in the form of an
	// array stored in col major order to set part of this matrix with the given range
	void set(std::vector<double> data_mIn, int r1, int r2, int c1, int c2, int ch1, int ch2)
	{
		if (r1 == -1) r1 = nr - 1;
		if (r2 == -1) r2 = nr - 1;
		if (c1 == -1) c1 = nc - 1;
		if (c2 == -1) c2 = nc - 1;
		if (ch1 == -1) ch1 = nch - 1;
		if (ch2 == -1) ch2 = nch - 1;

		int nr_new = r2 - r1 + 1;
		int nc_new = c2 - c1 + 1;
		int nch_new = ch2 - ch1 + 1;
		int ndata_per_chan_new = nr_new * nc_new;
		int ndata_new = ndata_per_chan_new * nch_new;

		if (ndata_new != data_mIn.size())
		{
			jni_utils ju(env);
			ju.throw_exception("ERROR from JNI: the input matrix data and the range specified do not match.");
			return;
		}			

		double* temp_in = data_mIn.data();
		double* temp_out = ptr_data;

		if (nr_new == nr && nc_new == nc)
		{
			std::copy(temp_in, temp_in + ndata_new, temp_out + ch1 * ndpch);
		}
		else if (nr_new == nr && nc_new != nc)
		{
			int cc = 0;
			for (int k = 0; k<nch_new; k++)
			{
				std::copy(temp_in + cc, temp_in + cc + ndata_per_chan_new, temp_out + (k + ch1) * ndpch + c1*nr);
				cc += nr_new * nc_new;
			}
		}
		else
		{
			int cc = 0;
			for (int k = 0; k<nch_new; k++)
				for (int j = 0; j<nc_new; j++)
				{
					std::copy(temp_in + cc, temp_in + cc + nr_new, temp_out + (k + ch1) * ndpch + (j + c1) * nr + r1);
					cc += nr_new;
				}
		}
	}

	// use the entire given input matrix to set part of this matrix with the given range
	void set(Matkc mIn, int r1, int r2, int c1, int c2)
	{
		set(mIn, r1, r2, c1, c2, 0, -1);
	}

	void set(std::vector<double> data_mIn, int r1, int r2, int c1, int c2)
	{
		set(data_mIn, r1, r2, c1, c2, 0, -1);
	}

	// use the entire given input matrix to set part of this matrix starting with i,j,k position
	void set(Matkc mIn, int i, int j, int k)
	{
		set(mIn, i, i + mIn.nr - 1, j, j + mIn.nc - 1, k, k + mIn.nch - 1);
	}

	// use the entire given input matrix to set part of this matrix starting with i,j,0 position
	void set(Matkc mIn, int i, int j)
	{
		set(mIn, i, i + mIn.nr - 1, j, j + mIn.nc - 1, 0, mIn.nch - 1);
	}

	// use the given value to set an element of this matrix at i,j,k position
	void set(double val, int i, int j, int k)
	{
		ptr_data[k * ndpch + j * nr + i] = val;
	}

	// use the given value to set an element of this matrix at i,j,0 position
	void set(double val, int i, int j)
	{
		ptr_data[j * nr + i] = val;
	}

	// use the given value to set an element of this matrix at lin_index linear position
	void set(double val, int lin_index)
	{
		ptr_data[lin_index] = val;
	}

	// use the given value to set an element of this matrix at 0,0,0 position
	void set(double val)
	{
		ptr_data[0] = val;
	}

	// use the entire given input matrix to set part of this matrix specified by row, col and chan indices.
	void set(Matkc mIn, std::vector<int> row_indices, std::vector<int> col_indices, std::vector<int> channel_indices)
	{
		int nr_new = row_indices.size();
		int nc_new = col_indices.size();
		int nch_new = channel_indices.size();

		double* temp_in = mIn.ptr_data;
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[channel_indices[k] * ndpch + col_indices[j] * nr + row_indices[i]] = temp_in[cc++];
	}

	void set(std::vector<double> data_mIn, std::vector<int> row_indices, std::vector<int> col_indices, std::vector<int> channel_indices)
	{
		int nr_new = row_indices.size();
		int nc_new = col_indices.size();
		int nch_new = channel_indices.size();

		double* temp_in = data_mIn.data();
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[channel_indices[k] * ndpch + col_indices[j] * nr + row_indices[i]] = temp_in[cc++];
	}

	// use the entire given input matrix (must be a row vector) to set a row of this matrix
	void set_row(Matkc mIn, int row_index)
	{
		set(mIn, row_index, row_index, 0, -1, 0, -1);
	}

	void set_row(std::vector<double> data_mIn, int row_index)
	{
		set(data_mIn, row_index, row_index, 0, -1, 0, -1);
	}

	// use the entire given input matrix to a range of rows of this matrix
	void set_rows(Matkc mIn, int start_index, int end_index)
	{
		set(mIn, start_index, end_index, 0, -1, 0, -1);
	}

	void set_rows(std::vector<double> data_mIn, int start_index, int end_index)
	{
		set(data_mIn, start_index, end_index, 0, -1, 0, -1);
	}

	// use the entire given input matrix to set specified rows of this matrix.
	void set_rows(Matkc mIn, std::vector<int> row_indices)
	{
		int nr_new = row_indices.size();

		double* temp_in = mIn.ptr_data;
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[k * ndpch + j * nr + row_indices[i]] = temp_in[cc++];
	}

	void set_rows(std::vector<double> data_mIn, std::vector<int> row_indices)
	{
		int nr_new = row_indices.size();

		double* temp_in = data_mIn.data();
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr_new; i++)
					temp_out[k * ndpch + j * nr + row_indices[i]] = temp_in[cc++];
	}

	// use the entire given input matrix (must be a col vector) to set a col of this matrix
	void set_col(Matkc mIn, int col_index)
	{
		set(mIn, 0, -1, col_index, col_index, 0, -1);
	}

	void set_col(std::vector<double> data_mIn, int col_index)
	{
		set(data_mIn, 0, -1, col_index, col_index, 0, -1);
	}

	// use the entire given input matrix to a range of cols of this matrix
	void set_cols(Matkc mIn, int start_index, int end_index)
	{
		set(mIn, 0, -1, start_index, end_index, 0, -1);
	}

	void set_cols(std::vector<double> data_mIn, int start_index, int end_index)
	{
		set(data_mIn, 0, -1, start_index, end_index, 0, -1);
	}

	// use the entire given input matrix to set specified cols of this matrix.
	void set_cols(Matkc mIn, std::vector<int> col_indices)
	{
		int nc_new = col_indices.size();

		double* temp_in = mIn.ptr_data;
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr; i++)
					temp_out[k * ndpch + col_indices[j] * nr + i] = temp_in[cc++];
	}

	void set_cols(std::vector<double> data_mIn, std::vector<int> col_indices)
	{
		int nc_new = col_indices.size();

		double* temp_in = data_mIn.data();
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch; k++)
			for (int j = 0; j<nc_new; j++)
				for (int i = 0; i<nr; i++)
					temp_out[k * ndpch + col_indices[j] * nr + i] = temp_in[cc++];
	}

	// use the entire given input matrix (must be a channel) to set a channel of this matrix
	void set_channel(Matkc mIn, int channel_index)
	{
		set(mIn, 0, -1, 0, -1, channel_index, channel_index);
	}

	void set_channel(std::vector<double> data_mIn, int channel_index)
	{
		set(data_mIn, 0, -1, 0, -1, channel_index, channel_index);
	}

	// use the entire given input matrix to a range of channels of this matrix
	void set_channels(Matkc mIn, int start_index, int end_index)
	{
		set(mIn, 0, -1, 0, -1, start_index, end_index);
	}

	void set_channels(std::vector<double> data_mIn, int start_index, int end_index)
	{
		set(data_mIn, 0, -1, 0, -1, start_index, end_index);
	}
	
	// use the entire given input matrix to set specified channels of this matrix.
	void set_channels(Matkc mIn, std::vector<int> channel_indices)
	{
		int nch_new = channel_indices.size();
		double* temp_in = mIn.ptr_data;
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr; i++)
					temp_out[channel_indices[k] * ndpch + j * nr + i] = temp_in[cc++];
	}

	void set_channels(std::vector<double> data_mIn, std::vector<int> channel_indices)
	{
		int nch_new = channel_indices.size();
		double* temp_in = data_mIn.data();
		double* temp_out = ptr_data;

		int cc = 0;
		for (int k = 0; k<nch_new; k++)
			for (int j = 0; j<nc; j++)
				for (int i = 0; i<nr; i++)
					temp_out[channel_indices[k] * ndpch + j * nr + i] = temp_in[cc++];
	}

	void print()
	{
		std::cout << "=========== Printing matrix ===========" << std::endl;
		for (int k = 0; k<nch; k++)
		{
			std::cout << "mat(:,:," << (k + 1) << ")=[" << std::endl;
			for (int i = 0; i<nr; i++)
			{
				for (int j = 0; j<nc - 1; j++)
					std::cout << get(i, j, k) << ",\t";		
				std::cout << get(i, nc - 1, k) << ";" << std::endl;
			}
			std::cout << "];" << std::endl;
		}
		std::cout << "=========== Matrix printed ===========" << std::endl;
	}

	void print_info()
	{
		printf("Matrix info: #rows = %d, #ncols = %d, #nchannels = %d\n", nr, nc, nch);
	}

	void print_info(std::string name_matrix)
	{
		printf("Matrix %s info: #rows = %d, #ncols = %d, #nchannels = %d\n", name_matrix.c_str(), nr, nc, nch);
	}

	void save_data(std::string fpath)
	{
		std::ofstream fid(fpath);
		fid << "Matkc data; nrows = " << nr << ", ncols = " << nc << ", nchannels = " << nch << std::endl;
		for (int i = 0; i < nd; i++)
			fid << ptr_data[i] << std::endl;
	}
			
	int nrows() const { return nr; }
	int ncols() const { return nc; }
	int nchannels () const { return nch; }
	int ndata_per_chan() const { return ndpch; }
	int ndata() const { return nd; }

	jobject get_obj() const { return obj; }
};

// wrap a Java class object so that I can get fields, set fields,
// and call methods in a convenient way
class JavaClass
{

private:

	JNIEnv* env;
	jclass cls;
	jobject obj;
	std::string str_class_sig;
	std::string str_classname;
	
public:

	// constructor given a fully-specified class name
	// e.g. KKH/StdLib/Matkc
	// does not create the object. Can use JavaClass.construct_new()
	// to create the object.
	JavaClass(JNIEnv* env_, std::string str_classname_)
	{
		env = env_;	
		str_classname = str_classname_;
		
		// if str_classname already includes "L...;" set it to str_class_sig and remove it
		// for storing in str_classname.
		// Otherwise, str_classname will remain as it is and str_class_sig will be
		// str_classname with "L" and ";" added to the beginning and end of it respectively
		// Actually just to note that this is not really necessary as FindClass and others
		// work work on either with "L...;" or without it. But the code below is just to 
		// keep things consistent.
		if (str_classname.find("L") == 0 && str_classname.find(";") == str_classname.size() - 1)
		{
			str_class_sig = str_classname;
			str_classname = str_classname.substr(1, str_classname.size() - 2);
		}
		else
		{
			str_class_sig = "L" + str_classname + ";";
		}
		
		cls = env->FindClass(str_classname.c_str());
	}
	
	// wraps an existing java object.
	// automatically computes the class signature, etc.
	JavaClass(JNIEnv* env_, jobject obj_)
	{
		env = env_;
		obj = obj_;
		cls = env->GetObjectClass(obj);
		jni_utils ju(env);
		str_class_sig = ju.get_signature_jobject(obj);
		str_classname = ju.get_signature_jobject(obj, true);
	}
	
	std::string get_classname()
	{
		return str_classname;
	}

	std::string get_class_sig()
	{
		return str_class_sig;
	}

	jobject get_obj()
	{
		return obj;
	}

	// creates a new object. This is normally used after constructing this class
	// with JavaClass(JNIEnv* env_, std::string str_classname_) which did not result in
	// a java object (just class info such as signature is stored).
	// types_args can be jint, jfloat, jdouble, jshort, jchar, jlong, jbyte and jstring
	// jintArray, jdoubleArray, jfloatArray, jshortArray, jcharArray, jlongArray, jbyteArray
	// it can also be jobject
	template<typename... types_args>
	void construct_new(types_args... args)
	{
		jni_utils ju(env);
		std::string sig_method = ju.get_signature_jmethod<void, types_args...>("", args...);

		// call constructor to create jobject
		jmethodID mID = env->GetMethodID(cls, "<init>", sig_method.c_str());
		obj = env->NewObject(cls, mID, args...);	
	}

	// get a data member (i.e. field) from the object of this class.
	// T can be jint, jfloat, ..., jintArray, jdoubleArray, ..., jstring
	// T can also be jobject. If so, signature_field_if_T_is_jobject should be set
	// to a valid signature string. If I have a jobject created before, I can use
	// jni_utils.get_signature_jobject<jobject>(jobject obj) to automatically
	// get the string signature.
	// if T is not jobject, then signature_field_if_T_is_jobject is not used
	// not therefore can be set to any (garbage) value such as "".
	template<class T>
	T get_field(std::string name_field, std::string signature_field_if_T_is_jobject, bool is_static_field = false)
	{
		jni_utils ju(env);
		std::string sig_field = get_signature_jtype<T>(signature_field_if_T_is_jobject);
		jfieldID fieldID;

		if (is_static_field)
		{
			fieldID = env->GetStaticFieldID(cls, name_field.c_str(), sig_field.c_str());
			GetStaticFieldFunctor<T> ff;
			return ff(env, cls, fieldID);
		}
	 
		fieldID = env->GetFieldID(cls, name_field.c_str(), sig_field.c_str());
		GetFieldFunctor<T> ff;
		return ff(env, obj, fieldID);		
	}
				
	// set a data member (i.e. field) from the object of this class.
	// T can be jint, jfloat, ..., jintArray, jdoubleArray, ..., jstring
	// T can also be jobject. 
	template<class T>
	void set_field(std::string name_field, T val, bool is_static_field = false)
	{
		jni_utils ju(env);
		std::string sig_field;
		if (std::is_same<T, jobject>::value)
			sig_field = get_signature_jobject(val);
		else
			sig_field = get_signature_jtype<T>("");

		jfieldID fieldID;
		
		if (is_static_field)
		{
			fieldID = env->GetStaticFieldID(cls, name_field.c_str(), sig_field);
			SetStaticFieldFunctor<T> ff;
			ff(env, cls, fieldID, val);
			return;
		}

		fieldID = env->GetFieldID(cls, name_field.c_str(), sig_field);
		SetFieldFunctor<T> ff;
		ff(env, obj, fieldID, val);		
	}
	
	// call a non-static or static member function of this class/object.
	// types_args can be jint, jfloat, jdouble, jshort, jchar, jlong, jbyte and jstring
	// jintArray, jdoubleArray, jfloatArray, jshortArray, jcharArray, jlongArray, jbyteArray
	// it can also be jobject. 
	// type_returnVal can be any of the above and also "void".
	// If type_returnVal is jobject, signature_return_type_if_type_returnVal_is_jobject should be set
	// to a valid signature string. If I have a jobject created before, I can use
	// jni_utils.get_signature_jobject<jobject>(jobject obj) to automatically
	// get the string signature. Apart from case of type_returnVal being jobject, the signatures of all other types
	// are generated automatically, including if there is a jobject in the input arguments.
	template<typename type_returnVal, typename... types_inputArgs>
	type_returnVal call_method(std::string methodName, std::string signature_return_type_if_type_returnVal_is_jobject, bool is_static_method, types_inputArgs... inputArgs)
	{
		jni_utils ju(env);

		if (methodName == "") methodName = "<init>"; // assume that it is constructor
		
		std::string sig_method = ju.get_signature_jmethod<type_returnVal, types_inputArgs...>(signature_return_type_if_type_returnVal_is_jobject, inputArgs...);

		std::cout << "sig_method = " << sig_method << std::endl;

		jmethodID mID;
		if (is_static_method)
		{
			mID = env->GetStaticMethodID(cls, methodName.c_str(), sig_method.c_str());
			callXStaticMethodFunctor<type_returnVal, types_args...> ff;
			if (std::is_same<type_returnVal, void>::value)
				ff(env, cls, mID, args...);
			else
				return ff(env, cls, mID, args...);
		}
			
		else
		{
			mID = env->GetMethodID(cls, methodName.c_str(), sig_method.c_str());
			callXMethodFunctor<type_returnVal, types_args...> ff;
			if (std::is_same<type_returnVal, void>::value)
				ff(env, obj, mID, args...);
			else
				return ff(env, obj, mID, args...);
		}			
	}

};

// class to easily and directly manipulate java arrays
// can also use to create a new java array or wrap an existing one.
// T_arr should be of type jdoubleArray, jintArray, etc.
// T should be the correponding primitive type:
// if T_arr is jdoubleArray, T should be jdouble, etc.
template<class T_arr, class T>
class jArray
{

private:

T_arr arr;
T* ptr_data;
JNIEnv* env;
int nr, nc, nch, nd, ndpch;
bool colMajor;
bool currently_holding_data;

template<class T2> void ReleaseArrayElements() {};
template<> void ReleaseArrayElements<jdoubleArray>() {env->ReleaseDoubleArrayElements(arr, ptr_data, 0);}	
template<> void ReleaseArrayElements<jfloatArray>() { env->ReleaseFloatArrayElements(arr, ptr_data, 0); }
template<> void ReleaseArrayElements<jintArray>() { env->ReleaseIntArrayElements(arr, ptr_data, 0); }
template<> void ReleaseArrayElements<jshortArray>() { env->ReleaseShortArrayElements(arr, ptr_data, 0); }
template<> void ReleaseArrayElements<jcharArray>() { env->ReleaseCharArrayElements(arr, ptr_data, 0); }
template<> void ReleaseArrayElements<jlongArray>() { env->ReleaseLongArrayElements(arr, ptr_data, 0); }
template<> void ReleaseArrayElements<jbyteArray>() { env->ReleaseByteArrayElements(arr, ptr_data, 0); }

template<class T2> void GetArrayElements() {};
template<> void GetArrayElements<jdoubleArray>() { ptr_data = env->GetDoubleArrayElements(arr, 0); }
template<> void GetArrayElements<jfloatArray>() { ptr_data = env->GetFloatArrayElements(arr, 0); }
template<> void GetArrayElements<jintArray>() { ptr_data = env->GetIntArrayElements(arr, 0); }
template<> void GetArrayElements<jshortArray>() { ptr_data = env->GetShortArrayElements(arr, 0); }
template<> void GetArrayElements<jcharArray>() { ptr_data = env->GetCharArrayElements(arr, 0); }
template<> void GetArrayElements<jlongArray>() { ptr_data = env->GetLongArrayElements(arr, 0); }
template<> void GetArrayElements<jbyteArray>() { ptr_data = env->GetByteArrayElements(arr, 0); }

template<class T2> void NewArray(int size) {};
template<> void NewArray<jdoubleArray>(int size) { arr = env->NewDoubleArray(size); }
template<> void NewArray<jfloatArray>(int size) { arr = env->NewFloatArray(size); }
template<> void NewArray<jintArray>(int size) { arr = env->NewIntArray(size); }
template<> void NewArray<jshortArray>(int size) { arr = env->NewShortArray(size); }
template<> void NewArray<jcharArray>(int size) { arr = env->NewCharArray(size); }
template<> void NewArray<jlongArray>(int size) { arr = env->NewLongArray(size); }
template<> void NewArray<jbyteArray>(int size) { arr = env->NewByteArray(size); }

// release an existing array elements so that JVM can move it
// or garbage collect it whenever it wants.
void release_existing_array()
{
	if (currently_holding_data)
		ReleaseArrayElements<T_arr>();
}

void set_pointer_to_array_elements()
{
	GetArrayElements<T_arr>();
}

void allocate(int size)
{
	NewArray<T_arr>(size);
}

public:

jArray() = delete;

jArray(JNIEnv* env_)
{
	env = env_;
	currently_holding_data = false;
}

// wrap an existing java array
void wrap(T_arr arr_)
{
	release_existing_array();
	arr = arr_;
	nd = env->GetArrayLength(arr);
	nr = nd; nc = nch = 1; ndpch = nr * nc;
	colMajor = true;
	set_pointer_to_array_elements();
	currently_holding_data = true;
}

// wrap an existing java array which can be interpreted as a matrix
void wrap(T_arr arr_, int nrows, int ncols, int nchannels, bool colMajor_=true)
{
	nd = env->GetArrayLength(arr_);
	if (nrows * ncols * nchannels != nd)
	{
		jni_utils ju(env);
		ju.throw_exception("ERROR from JNI: nrows * ncols * nchannels != length of array to be wrapped.");
		return;
	}
	release_existing_array();
	arr = arr_;
	nr = nrows; nc = ncols; nch = nchannels; ndpch = nr * nc;
	colMajor = colMajor_;
	set_pointer_to_array_elements();
	currently_holding_data = true;
}

// create new java array from given size
void create_new(int size)
{
	release_existing_array();
	allocate(size);
	set_pointer_to_array_elements();
	currently_holding_data = true;
	nr = size; nc = 1; nch = 1; nd = size; ndpch = nr * nc;
	colMajor = true;
}

// create new java array which can be interpreted as a matrix
void create_new(int nrows, int ncols, int nchannels, bool colMajor_ = true)
{
	nr = nrows; nc = ncols; nch = nchannels;
	nd = nr * nc * nch; 
	ndpch = nr * nc;
	release_existing_array();
	allocate(nd);
	set_pointer_to_array_elements();
	currently_holding_data = true;	
	colMajor = colMajor_;
}

~jArray()
{
	release_existing_array();
}

void set_val(T val, int idx)
{
	ptr_data[idx] = val;
}

void set_val(T val, int i, int j, int k)
{
	if(colMajor)
		ptr_data[k * nr * nc + j * nr + i] = val;
	else
		ptr_data[i * nch * nc + j * nch + k] = val;
}

// assume k=0 and nch = 1
void set_val(T val, int i, int j)
{
	if (colMajor)
		ptr_data[j * nr + i] = val;
	else
		ptr_data[i * nc + j] = val;
}

T get_val(int idx)
{
return ptr_data[idx];
}

T get_val(int i, int j, int k)
{
	if(colMajor)
		return ptr_data[k * nr * nc + j * nr + i];
	else
		return ptr_data[i * nch * nc + j * nch + k];
}

// assume k=0 and nch = 1
T get_val(int i, int j)
{
	if (colMajor)
		return ptr_data[j * nr + i];
	else
		return ptr_data[i * nc + j];
}
};



#endif