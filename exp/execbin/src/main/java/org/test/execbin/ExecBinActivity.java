package org.test.execbin;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.util.Log;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import android.content.Context;
import android.widget.Button;
import android.view.View;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;

public class ExecBinActivity extends Activity {

	/*static {
      System.loadLibrary("gnustl_shared");
	  }*/
	
	private static final String EXEC_NAME = "collector_backend"; //id";
	private static final String LIB1_NAME = "libsysanal.so";
	private static final String LIB2_NAME = "libgnustl_shared.so";
	private static final String TAG = "Collector";
	private static final String PID_NAME = EXEC_NAME + ".pid";

	private boolean mQuit;
	private LocalSocket mClient = null;//, mServer = null;
	private LocalServerSocket mServer = null;
	private Thread mThread = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mylayout);

		startSocketServer();
    }

    @Override
    public void onStart() {
        super.onStart();
    }

	public void storeClicked(View v) {
		loadAndStoreBinary(EXEC_NAME, false);
		//loadAndStoreBinary(LIB2_NAME, true);
		//loadAndStoreBinary(LIB1_NAME, true);
		//System.loadLibrary("gnustl_shared");
		//System.loadLibrary("sysanal");
	}

	public void execClicked(View v) {
		execBinary(false);
	}

	public void execRootClicked(View v) {
		execBinary(true);
	}

	public void stopClicked(View v) {
		if (mThread == null) {
			Log.e(TAG, "no running thread");
			return;
		}
		mQuit = true;
		Log.d(TAG, "tried to interrupt");
		try {
			// mServer.close(); - bug for now, will work some time...
			// https://code.google.com/p/android/issues/detail?id=29939
			connectToMyself();
			mThread.join();
			mThread = null;
		} catch (InterruptedException e) {
			Log.e(TAG, "join interrupted");
		}
			/*} catch (IOException e) {
			Log.d(TAG, "io error when stopping: " + e.toString());
			}*/
	}

	private void connectToMyself() {
		try {
			LocalSocket s = new LocalSocket(LocalSocket.SOCKET_STREAM);
			s.connect(new LocalSocketAddress("org.test.execbin"));
			s.close();
		} catch (IOException e) {
			Log.e(TAG, "error connecting to myself: " + e.toString());
		}
	}

	private void startSocketServer() {
		mQuit = false;
		mClient = null;
		mServer = null;

		try {
			mServer = new LocalServerSocket("org.test.execbin");
			//mServer = new LocalSocket(LocalSocket.SOCKET_DGRAM);
			if (mServer == null) {
				Log.e(TAG, "could not create local server socket");
				return;
			}			
			//mServer.bind(new LocalSocketAddress("org.test.execbin"));
			//mServer.setSoTimeout(1000);
		} catch (IOException e) {
			Log.d(TAG, "io error creating local socket: " + e.toString());
		}
		
		mThread = new Thread(new Runnable() {
                @Override
				public void run() {
					try {
						while (!mQuit) {
							if (mClient == null) {
								Log.d(TAG, "accepting...");
								mClient = mServer.accept();
								if (mClient != null) {
									Log.d(TAG, "incoming connection");
								}
							}
							else {
								Log.d(TAG, "(already has connected client)");
								Thread.sleep(1000);
							}
						}
						Log.d(TAG, "quit and close socket");
						mClient.close();
						mServer.close();
					} catch (IOException e) {
						Log.d(TAG, "io error: " + e.toString());
					} catch (InterruptedException e) {
						Log.d(TAG, "terminated");
					}
					mClient = null;
					mServer = null;
				}
			}, "CollectorSocketThread");

		mThread.start();
		Log.d(TAG, "thread started");
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

			File pid_file = new File(getFilesDir() + "/" + PID_NAME);
			pid_file.createNewFile();
			
			if (!isRoot)
				args = new String[]{ getFilesDir() + "/" + EXEC_NAME, pid_file.getAbsolutePath() };
			else
				args = new String[]{
					"su", "-c", getFilesDir() + "/" + EXEC_NAME, pid_file.getAbsolutePath() };

		
			/*Process p = Runtime.getRuntime().exec(args);

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				Log.d(TAG, "sleep 1 interrupted");
			}

			BufferedReader pid_reader = new BufferedReader(new FileReader(pid_file));
			String pid = pid_reader.readLine();
			Log.d(TAG, "native PID => " + pid);

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				Log.d(TAG, "sleep 2 interrupted");
			}

			Log.d(TAG, "killing...");
			android.os.Process.sendSignal(new Integer(pid), 10);
			Log.d(TAG, "killed");*/
			
			//outStream(p.getInputStream(), "output");
			//outStream(p.getErrorStream(), "errors");
		} catch (IOException e) {
			Log.e(TAG, "could not execute: " + e.toString());
		}
	}
}
