package ch.ulrichard.flightpred;

import javax.xml.parsers.*;
import java.util.Collections;
import java.util.Comparator;
import java.util.Vector;
import java.lang.Float;
import org.w3c.dom.*;
import java.net.*;
import android.widget.*;

public class XmlHandler {
	
	final URL   xmlurl_;
	
	public XmlHandler(String xmlUrl) {
		try{
			this.xmlurl_ = new URL(xmlUrl);
		}
		catch(MalformedURLException e) {
			throw new RuntimeException(e);
		}
	}

	public void update(TableLayout table) {
		try {
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			DocumentBuilder builder = factory.newDocumentBuilder();
			Document dom = builder.parse(xmlurl_.openConnection().getInputStream());
			Element root = dom.getDocumentElement();
			NodeList daytags = root.getElementsByTagName("day");
			Node today = daytags.item(0);
			NodeList sites = today.getChildNodes();
			
			Vector<TableRow> tmprows = new Vector<TableRow>();
			
			for(int i=0; i<sites.getLength(); ++i) {
				TableRow row = new TableRow(table.getContext());
				if(!(sites.item(i) instanceof Element))
					continue;
				Element site = (Element)sites.item(i);
				
				String name = site.getAttribute("name");
				TextView txname = new TextView(table.getContext());
				txname.setText(name);
				row.addView(txname);
				
				NodeList predvalues = site.getChildNodes();
				for(int j=0; j<predvalues.getLength(); j++) {
					Node nodval = predvalues.item(j);									
					if(nodval.getFirstChild() == null)
						continue;
					
					if(nodval.getNodeName().equalsIgnoreCase("num_flight")) {
						TextView txv = new TextView(table.getContext());
						txv.setText(nodval.getFirstChild().getNodeValue());
						row.addView(txv, 1);
					}
					else if(nodval.getNodeName().equalsIgnoreCase("max_dist")) {
						TextView txv = new TextView(table.getContext());
						txv.setText(nodval.getFirstChild().getNodeValue());
						row.addView(txv, 2);
					}
				} // for predvalues
					  
				try {
					TextView tv = (TextView)(row.getVirtualChildAt(2));
					if(tv.getText().length() > 0)
					{
						float dist = Float.parseFloat(tv.getText().toString());
						if(dist > 0.0)
							tmprows.add(row);
					}
				} catch(Exception e) {
					
				}
			} // for sites
			
			Collections.sort(tmprows, new Comparator<TableRow>(){
				public int compare(TableRow lhs, TableRow rhs){
					TextView tvl = (TextView)lhs.getVirtualChildAt(2);
					TextView tvr = (TextView)rhs.getVirtualChildAt(2);
					float fll = Float.parseFloat(tvl.getText().toString());
					float flr = Float.parseFloat(tvr.getText().toString());
					return Float.compare(flr, fll);
				}
			});
			
			for(int i=0; i<tmprows.size(); i++)
				table.addView(tmprows.get(i));
			
		} catch(Exception e) {
			throw new RuntimeException(e);		
		}
	}
}
