package ch.ulrichard.flightpred;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.*;
//import ch.ulrichard.flightpred.*;

public class sitetable extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        try {
        	loadPredDay();
        	Log.i("flightpred", "sucesfully loaded initial table");
        } catch (RuntimeException e) {
        	String msg = e.getLocalizedMessage();
        	Log.i("flightpred", msg);
        }
    }
    
    public void loadPredDay() {
//    	XmlHandler xmlh = new XmlHandler("http://192.168.2.5/flightpred/forecast.php");
    	XmlHandler xmlh = new XmlHandler("http://flightpred.homelinux.org/forecast.php");
    	TableLayout table = (TableLayout)findViewById(R.id.TableLayout01);
    	xmlh.update(table);
    }
    
}