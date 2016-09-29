package org.test.execbin;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.util.Log;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import android.content.Context;

public class ExecBinActivity extends Activity {
	private static final String EXEC_NAME = "id";
	private static final String TAG = "ExecBin";

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

	private boolean loadAndStoreBinary() {
		InputStream in = null;
		FileOutputStream fos = null;
		byte[] buf = new byte[65536];
		try {
			fos = openFileOutput(EXEC_NAME, Context.MODE_PRIVATE);
			in = getAssets().open(EXEC_NAME);
			int len;
			do {
				len = in.read(buf);
				if (len > 0)
					fos.write(buf, 0, buf.length);
			} while (len > 0);
			fos.flush();
			Log.d(TAG, "successfully saved asset file");
			return true;
		} catch (IOException e) {
			Log.d(TAG, "error reading/saving asset file" + e.getStackTrace());
			return false;
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (IOException e) {}
			}
			if (fos != null) {
				try {
					fos.close();
				} catch (IOException e) {}
			}
		}
	}

	private void execBinary() {
	}
}
