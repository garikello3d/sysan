package org.igor.sysanal;

import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import java.util.Random;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import java.io.IOException;
import java.io.FileDescriptor;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;

public class SysAnalService extends Service {
	private static final String TAG = "SysAnalService";
	private enum ThreadState { IDLE, STARTING, STARTED, STOPPING };
	private ThreadState mState = ThreadState.IDLE;
	private Thread mThread;

	private LocalSocket mClient = null;
	private LocalServerSocket mServer = null;
	private static final String SOCK_NAME = "org.igor.sysanal.socket";
	private static final String EXEC_NAME = "collector_backend";

	public class LocalBinder extends Binder {
        SysAnalService getService() {
            return SysAnalService.this;
        }
    }

	private final IBinder mBinder = new LocalBinder();

	@Override
    public void onCreate() {
	}

	@Override
    public void onDestroy() {
		stopGather();
	}

	@Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

	public void startGather() {
		if (mState == ThreadState.IDLE) {
			mState = ThreadState.STARTING;
			Log.d(TAG, "startGather: started successfully");
			startSocketThread();
			deployBackend(EXEC_NAME);
			runBackend();
		}
		else
			Log.e(TAG, "startGather: could not start, not idle");
	}

	public void stopGather() {
		if (mState == ThreadState.STARTED) {
			mState = ThreadState.STOPPING;
			Log.d(TAG, "stopGather: stopping");
			
			// mServer.close(); - bug for now, will work some time...
			// https://code.google.com/p/android/issues/detail?id=29939
			connectToMyselfAndClose();
		}
		else
			Log.e(TAG, "stopGather: could not start, not started");
	}

	public boolean isBackendRunning() {
		return mState == ThreadState.STARTED || mState == ThreadState.STARTING;
	}

	private void startSocketThread() {
		if (!createSocketServer()) {
			mState = ThreadState.IDLE;
			return;
		}
		
		mThread = new Thread(new Runnable() {
                @Override
				public void run() {
					mState = ThreadState.STARTED;
					InputStream istr = null;
					try {
						while (mState == ThreadState.STARTED) {
							if (mClient == null) {
								Log.d(TAG, "accepting...");
								mClient = mServer.accept();
								if (mClient != null) {
									Log.d(TAG, "incoming connection");
									mClient.setSoTimeout(3);
								}
							}
							else {
								Log.d(TAG, "(already has connected client)");
								istr = mClient.getInputStream();
								int r = 0;
								try {
									r = istr.read();
								} catch (IOException e) {
									// skip "try again" exception
								}
								if (r != -1)
									Thread.sleep(1000);
								else {
									Log.d(TAG, "client is disconnected");
									break;
								}
							}
						}
						Log.d(TAG, "quit and close socket");
						if (mClient != null)
							mClient.close();
						mServer.close();
					} catch (IOException e) {
						Log.d(TAG, "io error: " + e.toString());
					} catch (InterruptedException e) {
						Log.d(TAG, "terminated");
					}
					mClient = null;
					mServer = null;
					mState = ThreadState.IDLE;
				}
			}, "CollectorSocketThread");

		mThread.start();
	}

	private boolean deployBackend(String filename) {
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

	private boolean runBackend() {
		Log.d(TAG, "starting binary");
		final String[] args = new String[] { "su", "-c", getFilesDir() + "/" + EXEC_NAME };
		try {
			Process p = Runtime.getRuntime().exec(args);
			Log.d(TAG, "binary started");
			return true;
		} catch (IOException e) {
			Log.e(TAG, "could not execute: " + e.toString());
			return false;
		}
	}

	private boolean createSocketServer() {
		mClient = null;
		mServer = null;

		try {
			mServer = new LocalServerSocket(SOCK_NAME);
			if (mServer == null) {
				Log.e(TAG, "could not create local server socket");
				return false;
			}			
		} catch (IOException e) {
			Log.d(TAG, "io error creating local socket: " + e.toString());
			return false;
		}

		return true;
	}

	private void connectToMyselfAndClose() {
		try {
			LocalSocket s = new LocalSocket(LocalSocket.SOCKET_STREAM);
			s.connect(new LocalSocketAddress(SOCK_NAME));
			s.close();
		} catch (IOException e) {
			Log.e(TAG, "error connecting to myself: " + e.toString());
		}
	}

}
