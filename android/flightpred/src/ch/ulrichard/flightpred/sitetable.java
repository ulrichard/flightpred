package ch.ulrichard.flightpred;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.Vector;

import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.widget.*;
import android.widget.TabHost.TabSpec;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.OverlayItem;

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
        	loadPredData();
        	Log.i("flightpred", "sucesfully loaded initial table");
        } catch (RuntimeException e) {
        	String msg = e.getLocalizedMessage();
        	Log.i("flightpred", msg);
        }
    }
    
    public void loadPredData() {
    	try {    	
//	    	XmlHandler xmlh = XmlHandler.inst("http://192.168.2.5/flightpred/forecast.php");
	    	XmlHandler xmlh = XmlHandler.inst("http://flightpred.homelinux.org/forecast.php");
	    	xmlh.load();
	    	TreeMap<String, TreeMap<Date, Float>> preddatea = xmlh.getPredData();
	    	TableLayout table = (TableLayout)findViewById(R.id.TableLayout01);
	    	
	    	TreeMap<Date, Paint> preddates = new TreeMap<Date, Paint>();
	    	Paint pntRed = new Paint();
			pntRed.setARGB(255, 255, 0, 0);
			Paint pntGreen = new Paint();
			pntGreen.setARGB(255, 0, 255, 0);
			Paint pntBlue = new Paint();
			pntBlue.setARGB(255, 0, 0, 255);
			Date day = new Date();
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
			String todaysdtr = sdf.format(day);
			day = sdf.parse(todaysdtr);
			preddates.put(day, pntRed);
			Calendar cal = Calendar.getInstance();
			cal.setTime(day);
			cal.add(Calendar.DATE, 1);
			preddates.put(cal.getTime(), pntGreen);
			cal.add(Calendar.DATE, 1);
			preddates.put(cal.getTime(), pntBlue);
			
			TableRow row = (TableRow)table.getChildAt(0);
			SimpleDateFormat sdfs = new SimpleDateFormat("dd.MMM");
			int i=0;
			for(Map.Entry<Date, Paint> ent2 : preddates.entrySet()) {
				day = ent2.getKey();
				TextView txv = new TextView(table.getContext());
				txv.setPaintFlags(ent2.getValue().getFlags());
				String valstr = sdfs.format(day);
				txv.setText(valstr);
				txv.setPadding(5, 3, 5, 3);
				i = i + 1;
				row.addView(txv, i);
			}
			
			for(Map.Entry<String, TreeMap<Date, Float>> ent : preddatea.entrySet()) {
				row = new TableRow(table.getContext());
				Set<Date> days = ent.getValue().keySet();
				
				TextView txname = new TextView(table.getContext());
				txname.setText(ent.getKey());
				txname.setPadding(3, 3, 5, 3);
				row.addView(txname);
				
				i = 0;
				for(Map.Entry<Date, Paint> ent2 : preddates.entrySet()) {
					day = ent2.getKey();
					TextView txv = new TextView(table.getContext());
					if(days.contains(day)) {
						String valstr = String.format("%.2f km", ent.getValue().get(day));
						txv.setText(valstr);
						txv.setPadding(5, 3, 5, 3);
					}
					i = i + 1;
					row.addView(txv, i);
				}
				table.addView(row);
			}
    	} catch(Exception e) {
			throw new RuntimeException(e);		
		}
    }
    
    public void sortRows() {
    	/*
		Collections.sort(tmprows, new Comparator<TableRow>(){
			public int compare(TableRow lhs, TableRow rhs){
				TextView tvl = (TextView)lhs.getVirtualChildAt(2);
				TextView tvr = (TextView)rhs.getVirtualChildAt(2);
				float fll = Float.parseFloat(tvl.getText().toString());
				float flr = Float.parseFloat(tvr.getText().toString());
				return Float.compare(flr, fll);
			}
		});
*/
    }
    
}