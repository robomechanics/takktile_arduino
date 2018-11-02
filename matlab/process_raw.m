function [absolute_pressure_values, valid_reading] = process_raw(raw_data)
%process_raw: converts raw sensor data to pressure array
%   Takes in string cell array over I2C from arduino, then switches to
%   integer pressure and strips garbage characters
        valid_reading = true;
        ss=raw_data(2:end-1); % remove the outside brackets
        split_cell = strsplit(ss,',');
        
        if (cellfun('isempty', split_cell)) % signal but no data
            valid_reading = false;
            absolute_pressure_values = zeros(1, 5);
        end
        
        split_cell = erase(split_cell, '[');
        split_cell = erase(split_cell, ']');
        split_cell = erase(split_cell, ' ');
        absolute_pressure_values = str2num(char(split_cell));
end

