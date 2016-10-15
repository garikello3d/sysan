package org.igor.sysanal;

import android.app.Service;
import android.content.Intent;
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

public class SysAnalService extends Service {
	private static final String TAG = "SysAnalService";
	private enum ThreadState { IDLE, STARTING, STARTED, STOPPING };
	private ThreadState mState = ThreadState.IDLE;
	private Thread mThread;

	private LocalSocket mClient = null;
	private LocalServerSocket mServer = null;
	private static final String SOCK_NAME = "org.igor.sysanal.socket";

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
			createAndStartRealWorker();
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

	private void createAndStartRealWorker() {
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

	private void createAndStartFakeWorker() {
		mThread = new Thread(new Runnable() {
				@Override
				public void run() {
					mState = ThreadState.STARTED;
					while (mState != ThreadState.STOPPING) {
						Log.d(TAG, "(service is alive)");
						try {
							Thread.sleep(1500);
						} catch (InterruptedException e) {
							break;
						}
					}
					mState = ThreadState.IDLE;
				}
			});
		mThread.start();
	}
}
