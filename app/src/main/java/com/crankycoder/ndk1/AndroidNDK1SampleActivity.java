package com.crankycoder.ndk1;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.crankycoder.marisa.IntRecordTrie;

import java.io.File;
import java.util.Set;


public class AndroidNDK1SampleActivity extends ActionBarActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_android_ndk1_sample);

        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String fmt = "<" + new String(new char[100]).replace("\0", "i");
                IntRecordTrie recordTrie = new IntRecordTrie(fmt);
                File storeDir = new File(sdcardArchivePath());
                File f = new File(sdcardArchivePath() +"/toronto.record_trie");
                recordTrie.mmap(f.getAbsolutePath());
                String[] bssids = new String[]{"ccb255dd9fbe", "68b6fc3fbe19", "9094e439de3c"};

                Set<Integer> result = null;
                for (String k: bssids) {
                    if (result == null) {
                        result = recordTrie.getResultSet(k);
                    } else {
                        result.retainAll(recordTrie.getResultSet(k));
                    }
                }

                // This should should show [3917] for the toronto dataset.
                Log.i("libmarisa", "Found result: " + result.toString());

            }
        });

    }



    public static String sdcardArchivePath() {
        return Environment.getExternalStorageDirectory() + File.separator + "StumblerOffline";
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_android_ndk1_sample, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
