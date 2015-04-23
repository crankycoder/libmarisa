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

import java.io.File;
import java.io.IOException;
import java.util.List;


public class AndroidNDK1SampleActivity extends ActionBarActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_android_ndk1_sample);

        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                BytesTrie byteTrie = new BytesTrie();
                File storeDir = new File(sdcardArchivePath());
                Log.i("libmarisa", storeDir.getAbsolutePath() + " exists " + storeDir.exists());

                File f = new File(sdcardArchivePath() +"/benchmark.bytes_trie");
                Log.i("libmarisa", f.getAbsolutePath() + " exists " + f.exists());
                byteTrie.load(f.getAbsolutePath());
                Log.i("libmarisa", "BytesTrie is loaded!");

                List<byte[]> barResult = byteTrie.get("bar");
                if (barResult.size() == 1) {
                    Log.i("libmarisa", "Fetching [bar] key gets: [" + new String(barResult.get(0)) + "]") ;
                }

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
