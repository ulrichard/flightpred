package ch.ulrichard.flightpred;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.TreeMap;
import org.json.*;
import org.apache.http.*;
import org.apache.http.client.*;
import org.apache.http.impl.client.*;
import org.apache.http.client.methods.*;
import com.google.android.maps.GeoPoint;


public class JsonHandler extends DataHandlerBase {
	
	
	protected JsonHandler(String fileurl) {
		super(fileurl);
	}

	protected void load() {
		try {
			JSONObject json = getJSONObject(fileurl_.toString());		
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
			
			JSONArray days = json.getJSONArray("days");
			for(int i=0; i<days.length(); i++) {
				JSONObject dayn = days.getJSONObject(i);
				Date day = sdf.parse(dayn.getString("day"));
				JSONArray sites = dayn.getJSONArray("sites");
				for(int j=0; j<sites.length(); j++) {
					JSONObject site = sites.getJSONObject(j);
					String name = site.getString("site");
					String spos = site.getString("location"); // something like "POINT(8.11453333324856 46.411933333079)"
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
					
					if(!site.has("max_dist"))
						continue;
					float dist = (float)site.getDouble("max_dist");
					if(dist <= 0.0)
						continue;
					
					if(!preddata_.keySet().contains(name))
						preddata_.put(name, new TreeMap<Date, Float>());
					
					preddata_.get(name).put(day, dist);
				}
			}
		} catch(Exception e) {
			throw new RuntimeException(e);		
		}
	}
	
	private JSONObject getJSONObject(String url)
	{
		HttpClient httpClient = new DefaultHttpClient();
		HttpGet httpGet = new HttpGet(url);
	
		try {
			HttpResponse response = httpClient.execute(httpGet);
			// TODO: HTTP-Status (z.B. 404) in eigener Anwendung verarbeiten.
				
			HttpEntity entity = response.getEntity();
			if(entity != null) {
				InputStream instream = entity.getContent();
				BufferedReader reader = new BufferedReader(new InputStreamReader(instream));
				StringBuilder sb = new StringBuilder();
			
				String line = null;
				while ((line = reader.readLine()) != null)
					sb.append(line + "\n");
			
				String result = sb.toString();
			
				instream.close();
			
				JSONObject json = new JSONObject(result);
				return json;
			}
		}
		catch (ClientProtocolException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		} catch (JSONException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		} catch (Exception e){
			e.printStackTrace();
			throw new RuntimeException(e);
		}finally{
			httpGet.abort();
		}
		
		throw new RuntimeException("Could not download the json data file.");
	}
}
