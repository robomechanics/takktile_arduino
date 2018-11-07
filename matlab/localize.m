function [location, pressure_norm] = localize(gauge_pressure_values, pressure_threshold)
    num_sensors = size(gauge_pressure_values, 1);
    pressure_norm = norm(gauge_pressure_values);
    if (pressure_norm < pressure_threshold)
        location = 0;
        return
    end
    weights = (1:num_sensors)';
    location = sum(gauge_pressure_values.*weights)/sum(gauge_pressure_values);
end

