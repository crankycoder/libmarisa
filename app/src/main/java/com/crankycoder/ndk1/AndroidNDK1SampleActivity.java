package com.crankycoder.ndk1;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.crankycoder.marisa.BytesTrie;
import com.crankycoder.marisa.Record;
import com.crankycoder.marisa.RecordTrie;

import java.io.File;
import java.util.HashSet;
import java.util.List;
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
                RecordTrie recordTrie = new RecordTrie(fmt);
                File storeDir = new File(sdcardArchivePath());
                File f = new File(sdcardArchivePath() +"/toronto.record_trie");
                recordTrie.mmap(f.getAbsolutePath());

                String[] bssids = new String[]{"ccb255dd9fbe", "68b6fc3fbe19", "9094e439de3c"};

                tryFetch(recordTrie, bssids);

            }

            private Set<Integer> tryFetch(RecordTrie rTrie, String[] bssids) {
                Set<Integer> intersectedSet = null;
                for (String k: bssids) {
                    List<Record> curBatch = rTrie.getRecord(k);
                    for (Record record: curBatch) {
                        Set<Integer> tmp = new HashSet<Integer>();
                        Log.i("libmarisa", "----- start matchset ----- ");
                        for (int i = 0; i < record.size(); i++) {
                            Integer v = record.getInt(i);
                            tmp.add(v);
                            Log.i("libmarisa", "Tile match: ["+v+"]");
                        }
                        Log.i("libmarisa", "----- end matchset ----- ");

                        if (intersectedSet == null) {
                            intersectedSet = tmp;
                        } else {
                            intersectedSet.retainAll(tmp);
                        }
                    }
                }

                if (intersectedSet == null) {
                    intersectedSet = new HashSet<Integer>();
                }

                String foundMsg = "";
                for (Integer ik: intersectedSet) {
                    if (foundMsg.equals("")) {
                        foundMsg = ik.toString();
                    } else {
                        foundMsg += ", " + ik.toString();
                    }
                }
                Log.i("libmarisa", "Found results: [" +  foundMsg + "]");
                return intersectedSet;
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
