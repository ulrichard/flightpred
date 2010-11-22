package ch.ulrichard.flightpred;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.Date;
import java.util.TreeMap;
import com.google.android.maps.GeoPoint;

public abstract class DataHandlerBase {
	static private DataHandlerBase inst_ = null;
	final URL   fileurl_;
	protected TreeMap<String, TreeMap<Date, Float>> preddata_;
	protected TreeMap<String, GeoPoint>             locations_;
	
	protected DataHandlerBase(String fileurl) {
		try{
			this.fileurl_ = new URL(fileurl);
			this.preddata_  = new TreeMap<String, TreeMap<Date, Float>>();
			this.locations_ = new TreeMap<String, GeoPoint>();
		}catch(MalformedURLException e) {
			throw new RuntimeException(e);
		}
	}
	
	static public DataHandlerBase inst() {
		if(inst_ == null)
			inst_ = new JsonHandler("http://flightpred.homelinux.org/forecast.php?format=json");
//			inst_ = new JsonHandler("http://192.168.2.5/flightpred/forecast.php?format=json");
		inst_.load();
		return inst_;
	}
	
	final TreeMap<String, TreeMap<Date, Float>> getPredData() {
		return preddata_;
	}
	
	GeoPoint getLocation(String sitename) {
		return locations_.get(sitename);
	}
	
	abstract protected void load();
	
	
}
