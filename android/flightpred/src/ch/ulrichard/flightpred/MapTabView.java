package ch.ulrichard.flightpred;

import com.google.android.maps.MapActivity;
import android.os.Bundle;

public class MapTabView extends MapActivity {

	@Override
	protected void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		setContentView(R.layout.maptabview);
	 }
	
	@Override
	protected boolean isRouteDisplayed() {
		return false;
	}

}
