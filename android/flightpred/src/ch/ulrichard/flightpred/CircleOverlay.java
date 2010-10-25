package ch.ulrichard.flightpred;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.Projection;

public class CircleOverlay extends Overlay {
	final GeoPoint pos_;
	final double   radius_;
	final Paint    paint_;
	
	public CircleOverlay(GeoPoint pos, double radius, Paint paint) {
		this.pos_    = pos;
		this.radius_ = radius; // km
		this.paint_  = paint;
	}

	public void draw(Canvas canvas, MapView mapView, boolean shadow) {
        super.draw(canvas, mapView, shadow);
        
        Projection projection = mapView.getProjection();
        Point point = new Point();
        projection.toPixels(pos_, point);
        int rpix = (int)projection.metersToEquatorPixels((float)(radius_ * 1000.0));
        
        canvas.drawCircle(point.x, point.y, rpix, paint_);     
	}
}
