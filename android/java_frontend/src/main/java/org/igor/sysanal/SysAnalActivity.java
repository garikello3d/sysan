package org.igor.sysanal;

import android.app.Activity;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.TextView;
import android.widget.Button;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.view.View;


public class SysAnalActivity extends Activity {
	private TextView mStatusText;
	private Button mStartStopBtn;
	private static final String STATE_ISRUNNING = "IsBackendRunning";
	private static final String TAG = "SysAnalActivity";
	
	private SysAnalService mService = null;
	private boolean mIsBackendRunning = false;

	
	@Override
    public void onCreate(Bundle saved) {
        super.onCreate(saved);
		Log.d(TAG, "onCreate");
		
        setContentView(R.layout.main);
		mStatusText = (TextView)findViewById(R.id.status);
		mStartStopBtn = (Button)findViewById(R.id.start_stop);

		if (saved != null) {
			mIsBackendRunning = saved.getInt(STATE_ISRUNNING) == 0 ? false : true;
			Log.d(TAG, "onCreate: restored isBackendRunning = " + mIsBackendRunning);
		}
		updateControls(mIsBackendRunning);

		Intent intent = new Intent(this, SysAnalService.class);
		startService(intent);

		if (mService == null) {
			Log.d(TAG, "onCreate: no service connection, binding");
			bindService(
				new Intent(this, SysAnalService.class),
				mConnection,
				Context.BIND_AUTO_CREATE);
		}
    }

	@Override
	public void onSaveInstanceState(Bundle outState) {
		Log.d(TAG, "saving state");
		outState.putInt(STATE_ISRUNNING, mIsBackendRunning ? 1 : 0);
		super.onSaveInstanceState(outState);
	}

	@Override
	protected void onDestroy() {
		Log.d(TAG, "onDestroy");
		if (mService != null) {
			unbindService(mConnection);
			mService = null;
			Log.d(TAG, "onDestroy: unbound service");
		}
		super.onDestroy();
	}

	public void startStopClicked(View v) {
		if (mIsBackendRunning)
			mService.stopGather();
		else
			mService.startGather();
		queryBackendState(null);
	}

	public void queryBackendState(View v) {
		if (mService != null) {
			mIsBackendRunning = mService.isBackendRunning();
			updateControls(mIsBackendRunning);
		}
		else
			Log.e(TAG, "queryBackendStatus: service is null!");
	}
	
	private void updateControls(boolean isRunning) {
		mStatusText.setText(isRunning ?
			getString(R.string.be_status_running) :
			getString(R.string.be_status_idle));
		mStartStopBtn.setText(isRunning ?
			getString(R.string.start_stop_stop) :
			getString(R.string.start_stop_start));
		Log.d(TAG, "updateControls with " + isRunning);
	}

	private ServiceConnection mConnection = new ServiceConnection() {
            public void onServiceConnected(ComponentName className, IBinder service) {
                mService = ((SysAnalService.LocalBinder)service).getService();
				queryBackendState(null);
            }
            public void onServiceDisconnected(ComponentName className) {
                mService = null;
            }
        };
}
