# Header-only-modern-Cpp-tools-for-high-level-implementation-of-Java-Native-Interfaces
Header-only modern C++ tools for high-level implementation of Java Native Interfaces

During the architecture and implementation of Java applications in the fields of Computer Vision, Machine Learning and Artificial Intelligence (as well as in all other areas that require complex and time-consuming computations, or interfacing Java with existing C or C++ libraries), Java Native Interfaces (JNI) may need to be implemented. However programming JNI from the C++ side requires a lot of redundant coding, non-trivial data type conversions between Java types and C++ types, as well as between C++ types required by the existing C++ libraries. This project provides a set of tools that makes use of modern C++ (in particular C++11 and above) so that any new project in the future that needs to implement a JNI can conveniently program at a high level using the provided tools in this project. In particular, the project makes use of C++ type traits, templates, classes and other modern techniques in order to automate much of the commonalities, and tedious and complex work in the JNI implementation.

Firstly, the set of tools contains classes for wrapping the JNIEnv, and methods for converting between standard C++ types and JNI C++ data types such as:

- jstring to std::string, and vice versa
- jboolean to bool, and vice versa
- jint to int, and vice versa
- jfloat to float, and vice versa
- std::vector<std::string> to jobjectArray, and vice versa
- std::vector<int> to jintArray, and vice versa
- std::vector<float> to jfloatArray, and vice versa
- std::vector<double> to jdoubleArray, and vice versa
- std::vector<short> to jshortArray, and vice versa
- std::vector<unsigned short> to jcharArray, and vice versa
- std::vector<_int64> to jlongArray, and vice versa
- std::vector<signed char> to jbyteArray, and vice versa
- jXArray to C-style pointers, and vice-versa where X can be int, float, double, byte, etc.

Moreover, it contains other functionalities such as:

- throwing exceptions in a single line
- automatically getting signatures of any jobject
- generating a complete signature string for a java method
- convenient calling a java method of an object or class

The tools also contains a powerful class called "JavaClass" that wraps JNIEnv, jclass and jobject, and allows the following functionalities:

- Creating a new Java object using a variable number of arguments, from C++ code. Only the arguments for the constructor with Java JNI types such as jint, jfloat, jdoubleArray, jbyteArray and jobject need to be given. The signature of the method of the constructor, as well as getting method ID, etc. are obtained automatically.
- Getting a field of the java object, given only the name of the field (in std::string). The field is automatically returned as a C++ type. All other complex chain of operations done automatically by the wrapping class, such as getting signature of the field, deciding static or non-static field, getting the field ID (or static field ID), retrieving the field corresponding to the field ID, and converting the retrieved field to the corresponding C++ type.
- Setting a field of the java object, given only the name of the field (in std::string) and the value to set (of type jint, jfloat, jstring, jdoubleArray, jobject, etc.). As above, all the long complex chain of operations are automated.
- Calling a method of the java object, given only the name of the method (in std::string), the return type and the input arguments. All necessary tedious steps are performed automatically.

Finally, the tools contain a class named "jArray" to easily and directly manipulate java arrays. It can be used to create a new java array or wrap an existing one, and manipulate it at a high level.

https://kyaw.xyz/2017/12/10/header-modern-cpp-tools-high-level-implementation-java-native-interfaces-jni

Copyright (C) 2017 Kyaw Kyaw Htike @ Ali Abdul Ghafur. All rights reserved.

Dr. Kyaw Kyaw Htike @ Ali Abdul Ghafur

https://kyaw.xyz

