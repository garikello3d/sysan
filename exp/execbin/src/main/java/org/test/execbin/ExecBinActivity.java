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
	private static final String EXEC_NAME = "collector_backend"; //id";
	private static final String LIB1_NAME = "libsysanal.so";
	private static final String LIB2_NAME = "libgnustl_shared.so";
	private static final String TAG = "Collector";

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
		loadAndStoreBinary(EXEC_NAME, false);
		loadAndStoreBinary(LIB2_NAME, true);
		loadAndStoreBinary(LIB1_NAME, true);
		//System.loadLibrary("gnustl_shared");
		//System.loadLibrary("sysanal");
	}

	public void execClicked(View v) {
		execBinary(false);
	}

	public void execRootClicked(View v) {
		execBinary(true);
	}

	private boolean loadAndStoreBinary(String filename, boolean loadlib) {
		InputStream in = null;
		FileOutputStream fos = null;
		byte[] buf = new byte[65536];
		try {
			fos = openFileOutput(filename, Context.MODE_PRIVATE);
			in = getAssets().open(filename);
			int len;
			do {
				len = in.read(buf);
				if (len > 0)
					fos.write(buf, 0, buf.length);
			} while (len > 0);
			fos.flush();
			Log.d(TAG, "successfully saved asset file");

			String full_name = getFilesDir() + "/" + filename;
			File f = new File(full_name);
			if (!loadlib) {
				if (f.setExecutable(true))
					Log.d(TAG, "changed permission to execute");
				else
					Log.e(TAG, "could not change permission to execute");
			}
			else {
				System.load(full_name);
				Log.d(TAG, "loaded library " + full_name);
			}
			
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

	private void outStream(InputStream str, String prefix) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(str));
		String line;
		while ((line = reader.readLine()) != null) {
			Log.d(TAG, prefix + ": " + line);
		}
	}

	private void execBinary(boolean isRoot) {
		try {
			String[] args;
			Log.d(TAG, "starting binary");
			
			if (!isRoot)
				args = new String[]{ getFilesDir() + "/" + EXEC_NAME };
			else
				args = new String[]{ "su", "-c", getFilesDir() + "/" + EXEC_NAME };

			String[] env = new String[]{ "LD_LIBRARY_PATH=" + getFilesDir() };
				//	"LD_LIBRARY_PATH=/data/app/org.test.execbin-1/lib/arm64:/data/app/org.test.execbin-2/lib/arm64" };
			
			Process p = Runtime.getRuntime().exec(args, env);

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				Log.d(TAG, "sleep interrupted");
			}
			
			p.destroy();
			
			//outStream(p.getInputStream(), "output");
			//outStream(p.getErrorStream(), "errors");

			Log.d(TAG, "finished binary");
		} catch (IOException e) {
			Log.e(TAG, "could not execute: " + e.toString());
		}
	}
}
