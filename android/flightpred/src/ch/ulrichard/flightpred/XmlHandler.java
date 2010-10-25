package ch.ulrichard.flightpred;

import javax.xml.parsers.*;
import java.util.*;
import java.text.*;
import java.lang.Float;
import org.w3c.dom.*;
import java.net.*;
import com.google.android.maps.GeoPoint;


public class XmlHandler {
	static private XmlHandler inst_ = null;
	final URL   xmlurl_;
	private TreeMap<String, TreeMap<Date, Float>> preddata_;
	private TreeMap<String, GeoPoint>             locations_;
	
	private XmlHandler(String xmlUrl) {
		try{
			this.xmlurl_    = new URL(xmlUrl);
			this.preddata_  = new TreeMap<String, TreeMap<Date, Float>>();
			this.locations_ = new TreeMap<String, GeoPoint>();
		}
		catch(MalformedURLException e) {
			throw new RuntimeException(e);
		}
	}
	
	static public XmlHandler inst(String xmlUrl) {
		if(inst_ == null)
			inst_ = new XmlHandler(xmlUrl);
		return inst_;
	}
	
	final TreeMap<String, TreeMap<Date, Float>> getPredData() {
		return preddata_;
	}
	
	GeoPoint getLocation(String sitename) {
		return locations_.get(sitename);
	}

	public void load() {
		try {
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			DocumentBuilder builder = factory.newDocumentBuilder();
			Document dom = builder.parse(xmlurl_.openConnection().getInputStream());
			Element root = dom.getDocumentElement();
			
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
			
			NodeList daytags = root.getElementsByTagName("day");
			for(int i=0; i<daytags.getLength(); ++i) {
				if(!(daytags.item(i) instanceof Element))
					continue;
				Element dayn = (Element)daytags.item(i);
				Date day = sdf.parse(dayn.getAttribute("val"));
				NodeList sites = dayn.getChildNodes();
				
				for(int j=0; j<sites.getLength(); ++j) {
					if(!(sites.item(j) instanceof Element))
						continue;
					Element site = (Element)sites.item(j);
					String name = site.getAttribute("name");
					String spos = site.getAttribute("location"); // something like "POINT(8.11453333324856 46.411933333079)"
					spos = spos.substring(6, spos.length() - 1); // something like "8.11453333324856 46.411933333079"
					
					// there seems to be no WKT parser in android, so I do it the ugly way
					double lat = 0.0, lon = 0.0;
					StringTokenizer stok = new StringTokenizer(spos);
					if(stok.hasMoreTokens())
						lon = Double.parseDouble(stok.nextToken());
					if(stok.hasMoreTokens())
						lat = Double.parseDouble(stok.nextToken());					
					GeoPoint point = new GeoPoint((int)(lat * 1E6), (int)(lon * 1E6));
					locations_.put(name, point);
					
					NodeList predvalues = site.getChildNodes();
					for(int k=0; k<predvalues.getLength(); k++) {
						Node nodval = predvalues.item(k);									
						if(nodval.getFirstChild() == null)
							continue;
						
						if(nodval.getNodeName().equalsIgnoreCase("max_dist")) {
							String valstr = nodval.getFirstChild().getNodeValue();
							if(valstr.length() == 0)
								continue;
							try{
								float dist = Float.parseFloat(valstr);
								if(dist <= 0.0)
									continue;
								
								if(!preddata_.keySet().contains(name))
									preddata_.put(name, new TreeMap<Date, Float>());
								
								preddata_.get(name).put(day, dist);
							} catch(Exception e) {
								
							}
						}
					} // for predvalues
				} // for sites
			} // for days

		} catch(Exception e) {
			throw new RuntimeException(e);		
		}
	}
}
