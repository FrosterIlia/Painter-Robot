% min_vel = 700;
% max_vel = 3000;

% steps_acc = 1000; % number of steps we are accelerating

% target_vel = 1500; % desired linear speed

% x_tot = 7000; % example number of steps to go

syms x target_vel min_vel x_tot max_vel steps_acc;
% syms x target_vel max_vel;

v_acc(x) = min_vel + x * (max_vel - min_vel) / steps_acc; % acceleration function

v_dec(x) = max_vel - (steps_acc - x_tot + x) * (max_vel - min_vel) / steps_acc; % deceleration function

%Heaviside function method
% v(x) = heaviside(steps_acc - x) * v_acc(x) ...
% + (heaviside(x - steps_acc) - heaviside(x - (x_tot - steps_acc))) * max_vel ...
% + (heaviside(x - (x_tot - steps_acc)) - heaviside(x - x_tot)) * v_dec(x); % speed function
% 
% fplot(v(x), [0, x_tot - 1])

% time_total = simplify(int(1/v_acc, x, 0, steps_acc) ...
%     + int(1/max_vel, x, steps_acc, x_tot - steps_acc) ...
%     + int(1/v_dec, x, x_tot - steps_acc, x_tot));

% time_total = double(int(1 / v(x), x, 0, x_tot)) % total time it takes to travel

% Now trying to relate some of the variables with each other
% simplify(solve(int(1 / v(x), x, 0, x_tot) == x_tot / target_vel, max_vel));



%Distinct Function Method

% total_time = int(1 / v_acc(x), x, 0, steps_acc) ...
%     + int(1 / max_vel, x, steps_acc, x_tot - steps_acc) ...
%     + int(1 / v_dec(x), x, x_tot - steps_acc, x_tot);


% simplify(total_time) % Gives the same result as a heaviside method

% simplify(solve(simplify(total_time) == (x_tot / target_vel), min_vel)) % This kinda worked with steps_acc

time(max_vel) = steps_acc/(max_vel - min_vel) * (log(min_vel + steps_acc * (max_vel - min_vel) / steps_acc) - log(min_vel)) + ...
    log(max_vel * (x_tot - steps_acc)) - log(max_vel * steps_acc) + ...
    -steps_acc / (max_vel - min_vel) * (log(max_vel - (steps_acc - x_tot + x_tot) * (max_vel - min_vel) / steps_acc) - log(max_vel - (steps_acc - x_tot + (x_tot - steps_acc)) * (max_vel - min_vel) / steps_acc))

solve(time(max_vel) == x_tot / target_vel, max_vel)