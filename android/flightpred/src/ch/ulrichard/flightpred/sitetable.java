package ch.ulrichard.flightpred;

import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.*;
import android.widget.TabHost.TabSpec;

public class sitetable extends TabActivity {
	TabHost 	TabHost_;
	FrameLayout FrameLayout_;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        TabHost_ = getTabHost();
        TabHost_.addTab(TabHost_.newTabSpec("tab_test2").setIndicator("Details").setContent(R.id.TableLayout01));
        
        TabSpec tabSpec = TabHost_.newTabSpec("tab_test1");
        tabSpec.setIndicator("Map");
        Context ctx = this.getApplicationContext();
        Intent i = new Intent(ctx, MapTabView.class);
        tabSpec.setContent(i);
        TabHost_.addTab(tabSpec);
        
        TabHost_.setCurrentTab(0);
        
        try {
        	loadPredDay();
        	Log.i("flightpred", "sucesfully loaded initial table");
        } catch (RuntimeException e) {
        	String msg = e.getLocalizedMessage();
        	Log.i("flightpred", msg);
        }
    }
    
    public void loadPredDay() {
    	XmlHandler xmlh = new XmlHandler("http://192.168.2.5/flightpred/forecast.php");
//    	XmlHandler xmlh = new XmlHandler("http://flightpred.homelinux.org/forecast.php");
    	TableLayout table = (TableLayout)findViewById(R.id.TableLayout01);
    	xmlh.update(table);
    }
    
}