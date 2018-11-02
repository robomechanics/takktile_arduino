function [location, pressure_norm] = localize(gauge_pressure_values, pressure_threshold)
    num_sensors = size(gauge_pressure_values, 1);
    pressure_norm = norm(gauge_pressure_values);
    weights = gauge_pressure_values ./ pressure_norm;
    
    % check below line
    location = dot(1:num_sensors, weights);
    
    if (pressure_norm < pressure_threshold)
        location = 0;
    end
    
end

