/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * This Original Code has been modified by IBM Corporation. Modifications made
 * by IBM described herein are Copyright (c) International Business Machines
 * Corporation, 2000.
 * Modifications to Mozilla code or documentation identified per MPL Section 3.3
 *
 * Date             Modified by     Description of modification
 * 04/20/2000       IBM Corp.      OS/2 VisualAge build.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * This file is part of the Java-vendor-neutral implementation of LiveConnect
 *
 * It contains the code used to reflect Java methods as properties of
 * JavaObject objects and the code to invoke those methods.
 *
 */

#include <stdlib.h>
#include <string.h>

#include "jsj_private.h"        /* LiveConnect internals */
#include "jsjava.h"             /* LiveConnect external API */
#include "jsclist.h"            /* Circular linked lists */

/* A list of Java methods */
typedef JSCList MethodList;

/* An element in a list of Java methods */
typedef struct MethodListElement {
    JSCList linkage;
    JavaMethodSpec *method;
} MethodListElement;

/*
 * This is the return value of functions which compare either two types or two
 * method signatures to see if either is "preferred" when converting from
 * specified JavaScript value(s).
 */
typedef enum JSJTypePreference {
    JSJPREF_FIRST_ARG  = 1,       /* First argument preferred */
    JSJPREF_SECOND_ARG = 2,       /* Second argument preferred */
    JSJPREF_AMBIGUOUS  = 3        /* Neither preferred over the other */
} JSJTypePreference;

/*
 * Classification of JS types with slightly different granularity than JSType.
 * This is used to resolve among overloaded Java methods.
 */
typedef enum JSJType {
    JSJTYPE_VOID,                /* undefined */
    JSJTYPE_BOOLEAN,             /* boolean */
    JSJTYPE_NUMBER,              /* number */
    JSJTYPE_STRING,              /* string */
    JSJTYPE_NULL,                /* null */
    JSJTYPE_JAVACLASS,           /* JavaClass */
    JSJTYPE_JAVAOBJECT,          /* JavaObject */
    JSJTYPE_JAVAARRAY,		 /* JavaArray */
    JSJTYPE_JSARRAY,             /* JS Array */
    JSJTYPE_OBJECT,              /* Any other JS Object, including functions */
    JSJTYPE_LIMIT
} JSJType;

/*
 * A helper function for jsj_ConvertJavaMethodSignatureToString():
 * Compute JNI-style (string) signatures for an array of JavaSignature objects
 * and concatenate the results into a single string.
 *
 * If an error is encountered, NULL is returned and the error reporter is called.
 */
static const char *
convert_java_method_arg_signatures_to_string(JSContext *cx,
                                             JavaSignature **arg_signatures,
                                             int num_args)
{
    const char *first_arg_signature, *rest_arg_signatures, *sig;
    JavaSignature **rest_args;

    /* Convert the first method argument in the array to a string */
    first_arg_signature = jsj_ConvertJavaSignatureToString(cx, arg_signatures[0]);
    if (!first_arg_signature)
        return NULL;

    /* If this is the last method argument in the array, we're done. */
    if (num_args == 1)
        return first_arg_signature;

    /* Convert the remaining method arguments to a string */
    rest_args = &arg_signatures[1];
    rest_arg_signatures =
        convert_java_method_arg_signatures_to_string(cx, rest_args, num_args - 1);
    if (!rest_arg_signatures) {
        free((void*)first_arg_signature);
        return NULL;
    }

    /* Concatenate the signature string of this argument with the signature
       strings of all the remaining arguments. */
    sig = JS_smprintf("%s%s", first_arg_signature, rest_arg_signatures);
    free((void*)first_arg_signature);
    free((void*)rest_arg_signatures);
    if (!sig) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }

    return sig;
}

/*
 * A helper function for jsj_ConvertJavaMethodSignatureToHRString():
 * Compute human-readable string signatures for an array of JavaSignatures
 * and concatenate the results into a single string.
 *
 * If an error is encountered, NULL is returned and the error reporter is called.
 */
static const char *
convert_java_method_arg_signatures_to_hr_string(JSContext *cx,
                                                JavaSignature **arg_signatures,
                                                int num_args,
						JSBool whitespace)
{
    const char *first_arg_signature, *rest_arg_signatures, *sig, *separator;
    JavaSignature **rest_args;

    if (num_args == 0)
        return strdup("");

    /* Convert the first method argument in the array to a string */
    first_arg_signature = jsj_ConvertJavaSignatureToHRString(cx, arg_signatures[0]);
    if (!first_arg_signature)
        return NULL;

    /* If this is the last method argument in the array, we're done. */
    if (num_args == 1)
        return first_arg_signature;

    /* Convert the remaining method arguments to a string */
    rest_args = &arg_signatures[1];
    rest_arg_signatures =
        convert_java_method_arg_signatures_to_hr_string(cx, rest_args, num_args - 1, whitespace);
    if (!rest_arg_signatures) {
        free((void*)first_arg_signature);
        return NULL;
    }

    /* Concatenate the signature string of this argument with the signature
       strings of all the remaining arguments. */
    separator = whitespace ? " " : "";
    sig = JS_smprintf("%s,%s%s", first_arg_signature, separator, rest_arg_signatures);
    free((void*)first_arg_signature);
    free((void*)rest_arg_signatures);
    if (!sig) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }

    return sig;
}
/*
 * Compute a string signature for the given method using the same type names
 * that appear in Java source files, e.g. "int[][]", rather than the JNI-style
 * type strings that are provided by the functions above.  An example return
 * value might be "String MyFunc(int, byte, char[][], java.lang.String)".
 *
 * If an error is encountered, NULL is returned and the error reporter is called.
 */
const char *
jsj_ConvertJavaMethodSignatureToHRString(JSContext *cx,
                                         const char *method_name,
                                         JavaMethodSignature *method_signature)
{
    JavaSignature **arg_signatures, *return_val_signature;
    const char *arg_sigs_cstr;
    const char *return_val_sig_cstr;
    const char *sig_cstr;

    arg_signatures = method_signature->arg_signatures;
    return_val_signature = method_signature->return_val_signature;

    /* Convert the method argument signatures to a C-string */
    arg_sigs_cstr =
            convert_java_method_arg_signatures_to_hr_string(cx, arg_signatures,
                                                            method_signature->num_args,
							    JS_TRUE);
    if (!arg_sigs_cstr)
        return NULL;

    /* Convert the method return value signature to a C-string */
    return_val_sig_cstr = jsj_ConvertJavaSignatureToHRString(cx, return_val_signature);
    if (!return_val_sig_cstr) {
        free((void*)arg_sigs_cstr);
        return NULL;
    }

    /* Compose method arg signatures string and return val signature string */
    sig_cstr = JS_smprintf("%s %s(%s)", return_val_sig_cstr, method_name, arg_sigs_cstr);
    free((void*)arg_sigs_cstr);
    free((void*)return_val_sig_cstr);

    if (!sig_cstr) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }
    return sig_cstr;
}
/*
 * Destroy the sub-structure of a JavaMethodSignature object without free'ing
 * the method signature itself.
 */
void
jsj_PurgeJavaMethodSignature(JSContext *cx, JNIEnv *jEnv, JavaMethodSignature *method_signature)
{
    int i, num_args;
    JavaSignature **arg_signatures;

    if (!method_signature)  /* Paranoia */
        return;

    /* Free the method argument signatures */
    num_args = method_signature->num_args;
    arg_signatures = method_signature->arg_signatures;
    for (i = 0; i < num_args; i++)
        jsj_ReleaseJavaClassDescriptor(cx, jEnv, arg_signatures[i]);
    if (arg_signatures)
        JS_free(cx, arg_signatures);

    /* Free the return type signature */
    if (method_signature->return_val_signature)
        jsj_ReleaseJavaClassDescriptor(cx, jEnv, method_signature->return_val_signature);
}

/*
 * Fill in the members of a JavaMethodSignature object using the method
 * argument, which can be either an instance of either java.lang.reflect.Method
 * or java.lang.reflect.Constructor.
 *
 * With normal completion, return the original method_signature argument.
 * If an error occurs, return NULL and call the error reporter.
 */
JavaMethodSignature *
jsj_InitJavaMethodSignature(JSContext *cx, JNIEnv *jEnv,
                           jobject method,
                           JavaMethodSignature *method_signature)
{
    int i;
    jboolean is_constructor;
    jclass return_val_class;
    jsize num_args;
    JavaSignature *return_val_signature;
    jarray arg_classes;
    jmethodID getParameterTypes;

    memset(method_signature, 0, sizeof (JavaMethodSignature));
    
    is_constructor = (*jEnv)->IsInstanceOf(jEnv, method, jlrConstructor);

    /* Get a Java array that lists the class of each of the method's arguments */
    if  (is_constructor)
        getParameterTypes = jlrConstructor_getParameterTypes;
    else
        getParameterTypes = jlrMethod_getParameterTypes;
    arg_classes = (*jEnv)->CallObjectMethod(jEnv, method, getParameterTypes);
    if (!arg_classes) {
        jsj_UnexpectedJavaError(cx, jEnv,
                                "Can't determine argument signature of method");
        goto error;
    }

    /* Compute the number of method arguments */
    num_args = jsj_GetJavaArrayLength(cx, jEnv, arg_classes);
    if (num_args < 0)
        goto error;
    method_signature->num_args = num_args;

    /* Build a JavaSignature array corresponding to the method's arguments */
    if (num_args) {
        JavaSignature **arg_signatures;

        /* Allocate an array of JavaSignatures, one for each method argument */
        size_t arg_signatures_size = num_args * sizeof(JavaSignature *);
        arg_signatures = (JavaSignature **)JS_malloc(cx, arg_signatures_size);
        if (!arg_signatures)
            goto error;
        memset(arg_signatures, 0, arg_signatures_size);
        method_signature->arg_signatures = arg_signatures;
        
        /* Build an array of JavaSignature objects, each of which corresponds
           to the type of an individual method argument. */
        for (i = 0; i < num_args; i++) {
            JavaSignature *a;
            jclass arg_class = (*jEnv)->GetObjectArrayElement(jEnv, arg_classes, i);
            
            a = arg_signatures[i] = jsj_GetJavaClassDescriptor(cx, jEnv, arg_class);
            (*jEnv)->DeleteLocalRef(jEnv, arg_class);
            if (!a) {
                jsj_UnexpectedJavaError(cx, jEnv, "Could not determine Java class "
                                                  "signature using java.lang.reflect");
                goto error;
            }
        }
    }

    /* Get the Java class of the method's return value */
    if (is_constructor) {
        /* Constructors always have a "void" return type */
        return_val_signature = jsj_GetJavaClassDescriptor(cx, jEnv, jlVoid_TYPE);
    } else {
        return_val_class =
            (*jEnv)->CallObjectMethod(jEnv, method, jlrMethod_getReturnType);
        if (!return_val_class) {
            jsj_UnexpectedJavaError(cx, jEnv,
                                    "Can't determine return type of method "
                                    "using java.lang.reflect.Method.getReturnType()");
            goto error;
        }

        /* Build a JavaSignature object corresponding to the method's return value */
        return_val_signature = jsj_GetJavaClassDescriptor(cx, jEnv, return_val_class);
        (*jEnv)->DeleteLocalRef(jEnv, return_val_class);
    }

    if (!return_val_signature)
        goto error;
    method_signature->return_val_signature = return_val_signature;

    (*jEnv)->DeleteLocalRef(jEnv, arg_classes);
    return method_signature;

error:

    if (arg_classes)
        (*jEnv)->DeleteLocalRef(jEnv, arg_classes);
    jsj_PurgeJavaMethodSignature(cx, jEnv, method_signature);
    return NULL;
}

/*
 * Compute a JNI-style (string) signature for the given method, e.g. the method
 * "String MyFunc(int, byte)" is translated to "(IB)Ljava/lang/String;".
 *
 * If an error is encountered, NULL is returned and the error reporter is called.
 */
const char *
jsj_ConvertJavaMethodSignatureToString(JSContext *cx,
                                       JavaMethodSignature *method_signature)
{
    JavaSignature **arg_signatures, *return_val_signature;
    const char *arg_sigs_cstr;
    const char *return_val_sig_cstr;
    const char *sig_cstr;

    arg_signatures = method_signature->arg_signatures;
    return_val_signature = method_signature->return_val_signature;

    /* Convert the method argument signatures to a C-string */
    arg_sigs_cstr = NULL;
    if (arg_signatures) {
        arg_sigs_cstr =
            convert_java_method_arg_signatures_to_string(cx, arg_signatures,
                                                         method_signature->num_args);
        if (!arg_sigs_cstr)
            return NULL;
    }

    /* Convert the method return value signature to a C-string */
    return_val_sig_cstr = jsj_ConvertJavaSignatureToString(cx, return_val_signature);
    if (!return_val_sig_cstr) {
        free((void*)arg_sigs_cstr);
        return NULL;
    }

    /* Compose method arg signatures string and return val signature string */
    if (arg_sigs_cstr) {
        sig_cstr = JS_smprintf("(%s)%s", arg_sigs_cstr, return_val_sig_cstr);
        free((void*)arg_sigs_cstr);
    } else {
        sig_cstr = JS_smprintf("()%s", return_val_sig_cstr);
    }

    free((void*)return_val_sig_cstr);

    if (!sig_cstr) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }
    return sig_cstr;
}

static JSBool
add_java_method_to_class_descriptor(JSContext *cx, JNIEnv *jEnv,
                                    JavaClassDescriptor *class_descriptor, 
                                    jstring method_name_jstr,
                                    jobject java_method,
                                    JSBool is_static_method,
                                    JSBool is_constructor)
{
    jmethodID methodID;
    JSFunction *fun;
    jclass java_class = class_descriptor->java_class;

    JavaMemberDescriptor *member_descriptor = NULL;
    const char *sig_cstr = NULL;
    const char *method_name = NULL;
    JavaMethodSignature *signature = NULL;
    JavaMethodSpec **specp, *method_spec = NULL;
            
    if (is_constructor) {
        member_descriptor = jsj_GetJavaClassConstructors(cx, class_descriptor);
    } else {
        if (is_static_method) {
            member_descriptor = jsj_GetJavaStaticMemberDescriptor(cx, jEnv, class_descriptor, method_name_jstr);
        } else {
            member_descriptor = jsj_GetJavaMemberDescriptor(cx, jEnv, class_descriptor, method_name_jstr);
	}
        fun = JS_NewFunction(cx, jsj_JavaInstanceMethodWrapper, 0,
	                     JSFUN_BOUND_METHOD, NULL, member_descriptor->name);
	member_descriptor->invoke_func_obj = JS_GetFunctionObject(fun);
        JS_AddNamedRoot(cx, &member_descriptor->invoke_func_obj,
                        "&member_descriptor->invoke_func_obj");
    }
    if (!member_descriptor)
        return JS_FALSE;
    
    method_spec = (JavaMethodSpec*)JS_malloc(cx, sizeof(JavaMethodSpec));
    if (!method_spec)
        goto error;
    memset(method_spec, 0, sizeof(JavaMethodSpec));

    signature = jsj_InitJavaMethodSignature(cx, jEnv, java_method, &method_spec->signature);
    if (!signature)
        goto error;

    method_name = JS_strdup(cx, member_descriptor->name);
    if (!method_name)
        goto error;
    method_spec->name = method_name;
    
    sig_cstr = jsj_ConvertJavaMethodSignatureToString(cx, signature);
    if (!sig_cstr)
        goto error;

    if (is_static_method)
        methodID = (*jEnv)->GetStaticMethodID(jEnv, java_class, method_name, sig_cstr);
    else
        methodID = (*jEnv)->GetMethodID(jEnv, java_class, method_name, sig_cstr);
    method_spec->methodID = methodID;
    
    if (!methodID) {
        jsj_UnexpectedJavaError(cx, jEnv,
                                "Can't get Java method ID for %s.%s() (sig=%s)",
                                class_descriptor->name, method_name, sig_cstr);
        goto error;
    }
    
    JS_free(cx, (char*)sig_cstr);

    /* Add method to end of list of overloaded methods for this class member */ 
    specp = &member_descriptor->methods;
    while (*specp) {
        specp = &(*specp)->next;
    }
    *specp = method_spec;

    return JS_TRUE;

error:
    if (method_spec)
        JS_FREE_IF(cx, (char*)method_spec->name);
    JS_FREE_IF(cx, (char*)sig_cstr);
    if (signature)
        jsj_PurgeJavaMethodSignature(cx, jEnv, signature);
    JS_FREE_IF(cx, method_spec);
    return JS_FALSE;
}

JSBool 
jsj_ReflectJavaMethods(JSContext *cx, JNIEnv *jEnv,
                       JavaClassDescriptor *class_descriptor,
                       JSBool reflect_only_static_methods)
{
    jarray joMethodArray, joConstructorArray;
    jsize num_methods, num_constructors;
    int i;
    jclass java_class;
    JSBool ok, reflect_only_instance_methods;

    reflect_only_instance_methods = !reflect_only_static_methods;

    /* Get a java array of java.lang.reflect.Method objects, by calling
       java.lang.Class.getMethods(). */
    java_class = class_descriptor->java_class;
    joMethodArray = (*jEnv)->CallObjectMethod(jEnv, java_class, jlClass_getMethods);
    if (!joMethodArray) {
        jsj_UnexpectedJavaError(cx, jEnv,
                                "Can't determine Java object's methods "
                                "using java.lang.Class.getMethods()");
        return JS_FALSE;
    }

    /* Iterate over the class methods */
    num_methods = (*jEnv)->GetArrayLength(jEnv, joMethodArray);
    for (i = 0; i < num_methods; i++) {
        jstring method_name_jstr;
        
        /* Get the i'th reflected method */
        jobject java_method = (*jEnv)->GetObjectArrayElement(jEnv, joMethodArray, i);

        /* Get the method modifiers, eg static, public, private, etc. */
        jint modifiers = (*jEnv)->CallIntMethod(jEnv, java_method, jlrMethod_getModifiers);

        /* Don't allow access to private or protected Java methods. */
        if (!(modifiers & ACC_PUBLIC))
            goto dont_reflect_method;

        /* Abstract methods can't be invoked */
        if (modifiers & ACC_ABSTRACT)
            goto dont_reflect_method;

        /* Reflect all instance methods or all static methods, but not both */
        if (reflect_only_static_methods != ((modifiers & ACC_STATIC) != 0))
            goto dont_reflect_method;
        
        /* Add a JavaMethodSpec object to the JavaClassDescriptor */
        method_name_jstr = (*jEnv)->CallObjectMethod(jEnv, java_method, jlrMethod_getName);
        ok = add_java_method_to_class_descriptor(cx, jEnv, class_descriptor, method_name_jstr, java_method,
                                                 reflect_only_static_methods, JS_FALSE);
        (*jEnv)->DeleteLocalRef(jEnv, method_name_jstr);
        if (!ok) {
            (*jEnv)->DeleteLocalRef(jEnv, java_method);
            (*jEnv)->DeleteLocalRef(jEnv, joMethodArray);
            return JS_FALSE;
        }

dont_reflect_method:
        (*jEnv)->DeleteLocalRef(jEnv, java_method);
    }

    (*jEnv)->DeleteLocalRef(jEnv, joMethodArray);
    if (reflect_only_instance_methods)
        return JS_TRUE;
        
    joConstructorArray = (*jEnv)->CallObjectMethod(jEnv, java_class, jlClass_getConstructors);
    if (!joConstructorArray) {
        jsj_UnexpectedJavaError(cx, jEnv, "internal error: "
                                "Can't determine Java class's constructors "
                                "using java.lang.Class.getConstructors()");
        return JS_FALSE;
    }

    /* Iterate over the class constructors */
    num_constructors = (*jEnv)->GetArrayLength(jEnv, joConstructorArray);
    for (i = 0; i < num_constructors; i++) {
        /* Get the i'th reflected constructor */
        jobject java_constructor =
            (*jEnv)->GetObjectArrayElement(jEnv, joConstructorArray, i);

        /* Get the method modifiers, eg public, private, etc. */
        jint modifiers = (*jEnv)->CallIntMethod(jEnv, java_constructor,
                                                jlrConstructor_getModifiers);

        /* Don't allow access to private or protected Java methods. */
        if (!(modifiers & ACC_PUBLIC))
            goto dont_reflect_constructor;
        
        /* Add a JavaMethodSpec object to the JavaClassDescriptor */
        ok = add_java_method_to_class_descriptor(cx, jEnv, class_descriptor, NULL,
                                                 java_constructor,
                                                 JS_FALSE, JS_TRUE);
        if (!ok) {
            (*jEnv)->DeleteLocalRef(jEnv, joConstructorArray);
            (*jEnv)->DeleteLocalRef(jEnv, java_constructor);
            return JS_FALSE;
        }

dont_reflect_constructor:
        (*jEnv)->DeleteLocalRef(jEnv, java_constructor);
    }

    (*jEnv)->DeleteLocalRef(jEnv, joConstructorArray);
    return JS_TRUE;
}

/*
 * Free up a JavaMethodSpec and all its resources.
 */
void
jsj_DestroyMethodSpec(JSContext *cx, JNIEnv *jEnv, JavaMethodSpec *method_spec)
{
    if (!method_spec->is_alias) {
	JS_FREE_IF(cx, (char*)method_spec->name);
	jsj_PurgeJavaMethodSignature(cx, jEnv, &method_spec->signature);
    }
    JS_free(cx, method_spec);
}

static JavaMethodSpec *
copy_java_method_descriptor(JSContext *cx, JavaMethodSpec *method)
{
    JavaMethodSpec *copy;

    copy = (JavaMethodSpec*)JS_malloc(cx, sizeof(JavaMethodSpec));
    if (!copy)
        return NULL;
    memcpy(copy, method, sizeof(JavaMethodSpec));
    copy->next = NULL;
    copy->is_alias = JS_TRUE;
    return copy;
}

/*
 * See if a reference to a JavaObject/JavaClass's property looks like the
 * explicit resolution of an overloaded method, e.g. 
 * java.lang.String["max(double,double)"].  If so, add a new member
 * (JavaMemberDescriptor) to the object that is an alias for the resolved
 * method.
 */
JavaMemberDescriptor *
jsj_ResolveExplicitMethod(JSContext *cx, JNIEnv *jEnv,
			  JavaClassDescriptor *class_descriptor, 
			  jsid method_name_id,
			  JSBool is_static)
{
    JavaMethodSpec *method;
    JavaMemberDescriptor *member_descriptor;
    JavaMethodSignature *ms;
    JSString *simple_name_jsstr;
    JSFunction *fun;
    JSBool is_constructor;
    int left_paren;
    const char *sig_cstr, *method_name;
    char *arg_start;
    jsid id;
    jsval method_name_jsval;
      
    /*
     * Get the simple name of the explicit method, i.e. get "cos" from
     * "cos(double)", and use it to create a new JS string.
     */
    JS_IdToValue(cx, method_name_id, &method_name_jsval);
    method_name = JS_GetStringBytes(JSVAL_TO_STRING(method_name_jsval));
    arg_start = strchr(method_name, '(');	/* Skip until '(' */
    /* If no left-paren, then this is not a case of explicit method resolution */
    if (!arg_start)
	return NULL;
    /* Left-paren must be first character for constructors */
    is_constructor = (is_static && (arg_start == method_name));
        
    left_paren = arg_start - method_name;
    simple_name_jsstr = JS_NewStringCopyN(cx, method_name, left_paren);
    if (!simple_name_jsstr)
	return NULL;

    /* Find all the methods in the same class with the same simple name */
    JS_ValueToId(cx, STRING_TO_JSVAL(simple_name_jsstr), &id);
    if (is_constructor)
        member_descriptor = jsj_LookupJavaClassConstructors(cx, jEnv, class_descriptor);
    else if (is_static)
	member_descriptor = jsj_LookupJavaStaticMemberDescriptorById(cx, jEnv, class_descriptor, id);
    else
	member_descriptor = jsj_LookupJavaMemberDescriptorById(cx, jEnv, class_descriptor, id);
    if (!member_descriptor)	/* No member with matching simple name ? */
	return NULL;
    
    /*
     * Do a UTF8 comparison of method signatures with all methods of the same name,
     * so as to discover a method which exactly matches the specified argument types.
     */
    if (!strlen(arg_start + 1))
	return NULL;
    arg_start = JS_strdup(cx, arg_start + 1);
    if (!arg_start)
	return NULL;
    arg_start[strlen(arg_start) - 1] = '\0';	/* Get rid of ')' */
    sig_cstr = NULL;	/* Quiet gcc warning about uninitialized variable */
    for (method = member_descriptor->methods; method; method = method->next) {
	ms = &method->signature;
	sig_cstr = convert_java_method_arg_signatures_to_hr_string(cx, ms->arg_signatures,
								   ms->num_args, JS_FALSE);
	if (!sig_cstr)
	    return NULL;

	if (!strcmp(sig_cstr, arg_start))
	    break;
	JS_free(cx, (void*)sig_cstr);
    }
    JS_free(cx, arg_start);
    if (!method)
	return NULL;
    JS_free(cx, (void*)sig_cstr);
    
    /* Don't bother doing anything if the method isn't overloaded */
    if (!member_descriptor->methods->next)
	return member_descriptor;

    /*
     * To speed up performance for future accesses, create a new member descriptor
     * with a name equal to the explicit method name, i.e. "cos(double)".
     */
    member_descriptor = JS_malloc(cx, sizeof(JavaMemberDescriptor));
    if (!member_descriptor)
        return NULL;
    memset(member_descriptor, 0, sizeof(JavaMemberDescriptor));

    member_descriptor->id = method_name_id;
    member_descriptor->name =
        JS_strdup(cx, is_constructor ? "<init>" : JS_GetStringBytes(simple_name_jsstr));
    if (!member_descriptor->name) {
        JS_free(cx, member_descriptor);
        return NULL;
    }

    member_descriptor->methods = copy_java_method_descriptor(cx, method);
    if (!member_descriptor->methods) {
	JS_free(cx, (void*)member_descriptor->name);
        JS_free(cx, member_descriptor);
        return NULL;
    }
 
    fun = JS_NewFunction(cx, jsj_JavaInstanceMethodWrapper, 0,
			 JSFUN_BOUND_METHOD, NULL, method_name);
    member_descriptor->invoke_func_obj = JS_GetFunctionObject(fun);
    JS_AddNamedRoot(cx, &member_descriptor->invoke_func_obj,
                    "&member_descriptor->invoke_func_obj");

    /* THREADSAFETY */
    /* Add the new aliased member to the list of all members for the class */
    if (is_static) {
	member_descriptor->next = class_descriptor->static_members;
	class_descriptor->static_members = member_descriptor;
    } else {
	member_descriptor->next = class_descriptor->instance_members;
	class_descriptor->instance_members = member_descriptor;
    }

    return member_descriptor;
}

/*
 * Return the JavaScript types that a JavaScript method was invoked with
 * as a string, e.g. a JS method invoked like foo(3, 'hey', 'you', true)
 * would cause a return value of "(number, string, string, boolean)".
 * The returned string must be free'ed by the caller.
 * Returns NULL and reports an error if out-of-memory.
 */
static const char *
get_js_arg_types_as_string(JSContext *cx, uintN argc, jsval *argv)
{
    uintN i;
    const char *arg_type, *arg_string, *tmp;

    if (argc == 0)
        return strdup("()");
    
    arg_string = strdup("(");
    if (!arg_string)
        goto out_of_memory;
    for (i = 0; i < argc; i++) {
        arg_type = JS_GetTypeName(cx, JS_TypeOfValue(cx, argv[i]));
        tmp = JS_smprintf("%s%s%s%s", arg_string,  i ? ", " : "", arg_type,
                         (i == argc-1) ? ")" : "");
        free((char*)arg_string);
        if (!tmp)
            goto out_of_memory;
        arg_string = tmp;
    }

    return arg_string;

out_of_memory:
    JS_ReportOutOfMemory(cx);
    return NULL;
}

/*
 * This is an error reporting routine used when a method of the correct name
 * and class exists but either the number of arguments is incorrect or the
 * arguments are of the wrong type.
 */
static void
report_method_match_failure(JSContext *cx,
                            JavaMemberDescriptor *member_descriptor,
                            JavaClassDescriptor *class_descriptor,
                            JSBool is_static_method,
                            uintN argc, jsval *argv)
{
    const char *err, *js_arg_string, *tmp, *method_str, *method_name;
    JSBool is_constructor;
    JavaMethodSpec *method;

    err = NULL;
    is_constructor = (!strcmp(member_descriptor->name, "<init>"));

    js_arg_string = get_js_arg_types_as_string(cx, argc, argv);
    if (!js_arg_string)
        goto out_of_memory;

    if (is_constructor) {
        err =  JS_smprintf("There is no Java constructor for class %s that matches "
                           "JavaScript argument types %s.\n", class_descriptor->name,
                           js_arg_string);
        method_name = class_descriptor->name;
    } else {
        err =  JS_smprintf("There is no %sJava method %s.%s that matches "
                           "JavaScript argument types %s.\n",
                           is_static_method ? "static ": "",
                           class_descriptor->name, member_descriptor->name, js_arg_string);
        method_name = member_descriptor->name;
    }
    if (!err)
        goto out_of_memory;

    tmp = JS_smprintf("%sCandidate methods with the same name are:\n", err);
    if (!tmp)
        goto out_of_memory;
    err = tmp;

    method = member_descriptor->methods;
    while (method) {
        method_str =
            jsj_ConvertJavaMethodSignatureToHRString(cx, method_name, &method->signature);
        if (!method_str)
            goto out_of_memory;
        tmp = JS_smprintf("%s   %s\n", err, method_str);
        free((char*)method_str);
        if (!tmp)
            goto out_of_memory;
        err = tmp;

        method = method->next;
    }
    
    JS_ReportError(cx, err);
    return;

out_of_memory:
    if (js_arg_string)
        free((char*)js_arg_string);
    if (err)
        free((char*)err);
}

/*
 * This is an error reporting routine used when a method of the correct name
 * and class exists but more than one Java method in a set of overloaded
 * methods are compatible with the JavaScript argument types and none of them
 * match any better than all the others.
 */
static void
report_ambiguous_method_match(JSContext *cx,
                              JavaMemberDescriptor *member_descriptor,
                              JavaClassDescriptor *class_descriptor,
                              MethodList *ambiguous_methods,
                              JSBool is_static_method,
                              uintN argc, jsval *argv)
{
    const char *err, *js_arg_string, *tmp, *method_str, *method_name;
    JSBool is_constructor;
    JavaMethodSpec *method;
    MethodListElement *method_list_element;

    err = NULL;
    is_constructor = (!strcmp(member_descriptor->name, "<init>"));
