%/**************************************************************************/

%@file takktile_matlab.m
%@author Yaroslav Tenzer
%@license BSD

%Matlab example to capture data from the Arduino which is connected to the TakkTile Strip sensor

%This is a library for the TakkTile Strip sensor
%----> http://www.takktile.com/product:takkstrip

%@section HISTORY

%v1.0 - First release by Yaroslav Tenzer

%@section NOTES
% inspired partially by the code of Erin, the RobotGrrl 
% from http://robotgrrl.com/blog/2010/01/15/arduino-to-matlab-read-in-sensor-data/
%/**************************************************************************/

clear all; close all;
instrreset
% s = serial('COM5'); % for windows
% s = serial('/dev/ttyACM0'); % for ubuntu
s = serial('/dev/tty.usbmodem14201');

set(s,'Baudrate',115200);
set(s,'DataBits', 8);
set(s,'StopBits', 1);

fopen(s)
s.ReadAsyncMode = 'continuous';

% Start asynchronous reading
readasync(s);

num_sensors = 5;

%% Plotting
figure
plot_every = 20;
gauge_pressure_values = zeros(num_sensors, 1);
b = bar(gauge_pressure_values)
title('Pressure Values')
xlabel('Sensor Index')
ylabel('Pressure (ksi)')
ylim([0, 50])
b.YDataSource = 'gauge_pressure_values';

%% Calibration
num_calibration_samples = 100;
valid_readings = 0;
calibrated_zeroes = zeros(num_sensors, 1);
while(valid_readings < num_calibration_samples)
        tic
        tline1 = fscanf(s, '%s');
        
        [absolute_pressure_values, valid_reading] = process_raw(tline1);
        if (valid_reading == false)
            continue
        end
        calibrated_zeroes = calibrated_zeroes + absolute_pressure_values;
        valid_readings = valid_readings + 1;
end
calibrated_zeroes = calibrated_zeroes./num_calibration_samples


%% Sample from Sensors
try
    
for i=1:10000
        tic
        tline1 = fscanf(s, '%s');
        [absolute_pressure_values, valid_reading] = process_raw(tline1);
        if (valid_reading == false)
            continue;
        end
        gauge_pressure_values = absolute_pressure_values - calibrated_zeroes
        if (mod(i, plot_every) == 0)
            refreshdata
            drawnow
        end
        toc
end
%% ----

catch err
%        rethrow(err);
    err
end

%% --
stopasync(s);
fclose(s)
