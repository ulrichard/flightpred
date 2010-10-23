package ch.ulrichard.flightpred;

import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.Vector;
import org.json.*;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
//import org.apache.commons.io.*

public class JsonHandler {
final URL   fileurl_;
	
	public JsonHandler(String fileurl) {
		try{
			this.fileurl_ = new URL(fileurl);
		}
		catch(MalformedURLException e) {
			throw new RuntimeException(e);
		}
	}

	public void update(TableLayout table) {
		try {
//			String jsontext = IOUtils.toString(fileurl_.openConnection().getInputStream());
//			JSONObject jsonobj = new JSONObject.(jsontext);
					
			TreeMap<String, TreeMap<Date, Float>> siteinfos = new TreeMap<String, TreeMap<Date, Float>>();
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
			
/*			
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
								
								if(!siteinfos.keySet().contains(name))
									siteinfos.put(name, new TreeMap<Date, Float>());
								
								siteinfos.get(name).put(day, dist);
							} catch(Exception e) {
								
							}
						}
					} // for predvalues
				} // for sites
			} // for days

			
			Vector<Date> preddates = new Vector<Date>();
			Date day = new Date();
			String todaysdtr = sdf.format(day);
			day = sdf.parse(todaysdtr);
			preddates.add(day);
			Calendar cal = Calendar.getInstance();
			cal.setTime(day);
			cal.add(Calendar.DATE, 1);
			preddates.add(cal.getTime());
			cal.add(Calendar.DATE, 1);
			preddates.add(cal.getTime());
			
			TableRow row = (TableRow)table.getChildAt(0);
			SimpleDateFormat sdfs = new SimpleDateFormat("dd.MMM");
			for(int i=0; i<preddates.size(); i++) {
				day = preddates.get(i);
				TextView txv = new TextView(table.getContext());
				String valstr = sdfs.format(day);
				txv.setText(valstr);
				txv.setPadding(3, 3, 3, 3);
				row.addView(txv, i + 1);
			}
			
			for(Map.Entry<String, TreeMap<Date, Float>> ent : siteinfos.entrySet()) {
				row = new TableRow(table.getContext());
				
				TextView txname = new TextView(table.getContext());
				txname.setText(ent.getKey());
				txname.setPadding(3, 3, 3, 3);
				row.addView(txname);
				
				for(int i=0; i<preddates.size(); i++) {
					day = preddates.get(i);
					Set<Date> days = ent.getValue().keySet();
					TextView txv = new TextView(table.getContext());
					if(days.contains(day)) {
						String valstr = String.format("%.2f", ent.getValue().get(day));
						txv.setText(valstr);
						txv.setPadding(3, 3, 3, 3);
					}
					row.addView(txv, i + 1);
				}
				
				table.addView(row);
			}
*/	
		} catch(Exception e) {
			throw new RuntimeException(e);		
		}
	}
}
