package com.dreamliner.signjni;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import java.io.UnsupportedEncodingException;

import static android.content.pm.PackageManager.GET_SIGNATURES;
import static com.dreamliner.signjni.AESJniUtil.stringFromJNI;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        String encodeStr = AESJniUtil.encode("陈仲锦");
        Log.i("TAG", AESJniUtil.decode(encodeStr));
    }

    //用来获取你的签名的相关信息,然后coyp到你自己的cpp下
    private void getSignMes() {
        try {
            String packageName = getApplication().getPackageName();
            PackageManager packageManager = getApplication().getPackageManager();
            PackageInfo packageInfo = packageManager.getPackageInfo(packageName, GET_SIGNATURES);
            Signature[] signatures = packageInfo.signatures;
            String result = signatures[0].toCharsString();
            result = new String(result.getBytes("UTF-8"), "UTF-8");
            System.out.println(result);
        } catch (PackageManager.NameNotFoundException | UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }
}
