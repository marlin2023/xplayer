package com.cmcm.v.cmplayersdk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        XPlayer xPlayer  = new XPlayer();
        xPlayer.sayHello();

    }
}

