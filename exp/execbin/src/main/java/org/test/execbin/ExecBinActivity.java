package org.test.execbin;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class ExecBinActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mylayout);
    }

    @Override
    public void onStart() {
        super.onStart();
        TextView textView = (TextView) findViewById(R.id.text_view);
        textView.setText("Hello world!");
    }

}
