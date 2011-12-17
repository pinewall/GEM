load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/contributed.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_code.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_csm.ncl"
   
begin

values = readAsciiTable("value", 4, "float", (/1, 0/))  ;   values on grid
;printVarSummary(values)

cdf = addfile("../test.nc", "r")                        ;   open netCDF file for lat/lon information
dst_dims = cdf->dst_grid_dims;
;printVarSummary(dims)
nlat = dst_dims(1);
nlon = dst_dims(0);
;print(nlat);
;print(nlon);

lat1d = cdf->dst_grid_center_lat;
lon1d = cdf->dst_grid_center_lon;
lat2d = onedtond(lat1d, (/nlat, nlon/));
lon2d = onedtond(lon1d, (/nlat, nlon/));

; lat/lon for drawing
lats=lat2d(:,0);
lons=lon2d(0,:);
lats@units="degrees_north";
lons@units="degrees_east";

; values to draw
af=values(:,0);
o1=values(:,1);
o2a=values(:,2);
o2d=values(:,3);
o1_af=o1-af;
o2a_af=o2a-af;
o2d_af=o2d-af;
o2d_o2a=o2d-o2a;

; from 1D to 2D
af2d = onedtond(af, (/nlat, nlon/));
af2d!0 = "lat";
af2d!1 = "lon";
af2d&lat = lats;
af2d&lon = lons;
;af2d@lat2d=lat2d;
;af2d@lon2d=lon2d;

o1_af2d = onedtond(o1_af, (/nlat, nlon/));
o1_af2d!0 = "lat";
o1_af2d!1 = "lon";
o1_af2d&lat = lats;
o1_af2d&lon = lons;
printVarSummary(o1_af2d);

o2a_af2d = onedtond(o2a_af, (/nlat, nlon/));
o2a_af2d!0 = "lat";
o2a_af2d!1 = "lon";
o2a_af2d&lat = lats;
o2a_af2d&lon = lons;

o2d_af2d = onedtond(o2d_af, (/nlat, nlon/));
o2d_af2d!0 = "lat";
o2d_af2d!1 = "lon";
o2d_af2d&lat = lats;
o2d_af2d&lon = lons;

o2d_o2a2d = onedtond(o2d_o2a, (/nlat, nlon/));
o2d_o2a2d!0 = "lat";
o2d_o2a2d!1 = "lon";
o2d_o2a2d&lat = lats;
o2d_o2a2d&lon = lons;
;printVarSummary(af2d);

print("Finish data preparation");
; create plot: reference="www.ncl.ucar.edu/Applications/Scripts/proj_1.ncl"

res                     = True;
res@mpProjection        = "CylindricalEquidistant";
res@mpGridAndLimbOn     = True;
res@mpPerimOn           = False;
res@mpGridLatSpacingF   = 30.;
res@mpGridLonSpacingF   = 30.;
res@mpFillOn            = False;

res@mpLeftCornerLatF    = lats(0);
res@mpLeftCornerLonF    = lons(0);
res@mpRightCornerLatF   = lats(nlat-1);
res@mpRightCornerLonF   = lons(nlon-1);

res@mpRelativeCenterLon = True;
res@mpCenterLonF        = 0;
res@mpCenterLatF        = 0;
res@tfDoNDCOverlay      = False;
res@cnFillOn            = True;
;res@cnLineLabelOn       = False;
res@txFontHeightF       = 0.015;

res@vpXF                = 0.1;
res@vpYF                = 0.9;
res@vpWidthF            = 0.8;
res@vpHeightF           = 0.9;

res@gsnSpreadColors     = True;
res@gsnSpreadColorStart = 2;
res@gsnSpreadColorEnd   = -3;
res@lbLabelFontHeightF  = 0.015

res@tiMainString        = "Analytic function";
res@tiMainFontHeightF   = 0.018

;wks1 = gsn_open_wks("ps", "af");
wks2 = gsn_open_wks("ps", "o1_af");
wks3 = gsn_open_wks("ps", "o2a_af");
wks4 = gsn_open_wks("ps", "o2d_af");
wks5 = gsn_open_wks("ps", "o2d_o2a");

;gsn_define_colormap(wks1, "gui_default");
gsn_define_colormap(wks2, "gui_default");
gsn_define_colormap(wks3, "gui_default");
gsn_define_colormap(wks4, "gui_default");
gsn_define_colormap(wks5, "gui_default");

;print("Draw graph 1");
;contour = gsn_csm_contour_map(wks1, af2d(:,:), res);

print("Draw graph 2");
contour = gsn_csm_contour_map(wks2, o1_af2d(:,:), res);

print("Draw graph 3");
contour = gsn_csm_contour_map(wks3, o2a_af2d(:,:), res);

print("Draw graph 4");
contour = gsn_csm_contour_map(wks4, o2d_af2d(:,:), res);

print("Draw graph 5");
contour = gsn_csm_contour_map(wks5, o2d_o2a2d(:,:), res);

end