package org.test.execbin;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.util.Log;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import android.content.Context;
import android.widget.Button;
import android.view.View;

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
    }

	public void storeClicked(View v) {
		loadAndStoreBinary();
	}

	public void execClicked(View v) {
		execBinary(false);
	}

	public void execRootClicked(View v) {
		execBinary(true);
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

			File f = new File(getFilesDir() + "/" + EXEC_NAME);
			if (f.setExecutable(true))
				Log.d(TAG, "changed permission to execute");
			else
				Log.e(TAG, "could not change permission to execute");
			
			return true;
		} catch (IOException e) {
			Log.d(TAG, "error reading/saving asset file: " + e.toString());
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

	private void execBinary(boolean isRoot) {
		try {
			String[] args;
			
			if (!isRoot)
				args = new String[]{ getFilesDir() + "/" + EXEC_NAME };
			else
				args = new String[]{ "su", "-c", getFilesDir() + "/" + EXEC_NAME };
			
			Process p = Runtime.getRuntime().exec(args);

			BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
			String line;
			while ((line = reader.readLine()) != null) {
				Log.d(TAG, "output: " + line);
			}
		} catch (IOException e) {
			Log.e(TAG, "could not execute: " + e.toString());
		}
	}
}
