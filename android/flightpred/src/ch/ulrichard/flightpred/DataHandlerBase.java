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
	private   Date lastupdate_;
	
	protected DataHandlerBase(String fileurl) {
		try{
			this.fileurl_    = new URL(fileurl);
			this.lastupdate_ = new Date((new Date()).getTime() - 60 * 60 * 1000);
			this.preddata_   = new TreeMap<String, TreeMap<Date, Float>>();
			this.locations_  = new TreeMap<String, GeoPoint>();
		}catch(MalformedURLException e) {
			throw new RuntimeException(e);
		}
	}
	
	static public DataHandlerBase inst() {
		
		if(inst_ == null)
			inst_ = new JsonHandler("http://flightpred.ulrichard.ch/forecast.php?format=json");
		
		final Date tenMinAgo = new Date((new Date()).getTime() - 10 * 60 * 1000);
		if(inst_.lastupdate_.before(tenMinAgo)) // reload if the data is older then 10 minutes
			inst_.doload();

		return inst_;
	}
	
	final TreeMap<String, TreeMap<Date, Float>> getPredData() {
		return preddata_;
	}
	
	GeoPoint getLocation(String sitename) {
		return locations_.get(sitename);
	}
	
	abstract protected void load();
	
	private void doload() {
		load();
		lastupdate_ = new Date();
	}
}
