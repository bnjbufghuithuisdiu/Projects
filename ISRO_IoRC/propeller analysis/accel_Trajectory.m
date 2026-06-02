function velocity_profile_variable_slope(filename,mu)

data = readmatrix(filename);

x = data(:,1);
y = data(:,2);

g = 9.81;

% Maximum height
Hmax = max(y);

% slope
dy = gradient(y);
dx = gradient(x);
slope = dy ./ dx;

% slope angle
theta = atan(slope);

% arc length element
ds = sqrt(1 + slope.^2).*dx;

% cumulative friction work
%                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         n     n                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
friction_integral = cumsum(mu*g*cos(theta).*ds);

% velocity
v = sqrt(2*g*(Hmax - y) - 2*friction_integral);

% avoid imaginary values
v(v<0) = 0;

% results table
T = table(x,y,slope,theta,v);
disp(T)

% velocity plot
figure
plot(x,v,'LineWidth',2)
xlabel('Distance from max height')
ylabel('Velocity')
title('Velocity Profile on Variable Slope Wedge with Friction')
grid on

end
velocity_profile_variable_slope("TrajectoryData.xlsx",0.5)