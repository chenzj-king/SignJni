package com.dreamliner.signjni;

public class AESJniUtil {

    static {
        System.loadLibrary("native-lib");
    }

    public static native String stringFromJNI();

    public static native String encode(String plainText);

    public static native String decode(String cipherText);
}
