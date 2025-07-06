#pragma once
#include <Arduino.h>

class BasePlot
{
public:
    virtual ~BasePlot() = default;
    virtual String generate_command() = 0;
};

template <uint8_t parameters_number, typename T = float>
class Plot : public BasePlot
{
public:
    Plot(const char *name)
    {
        _name = name;
    };

    ~Plot() = default;

    typedef struct
    {
        const char *name;
        T *value;
    } Parameter;

    void attach_parameter(const char *name, T *value)
    {
        if (_parameter_count >= parameters_number)
            return;
        Parameter new_parameter = {name, value};
        _parameters[_parameter_count++] = new_parameter;
    };

    String generate_command()
    {
        String command = _name;
        command.reserve(64); // TODO: Remove magic numbers
        for (uint8_t i = 0; i < parameters_number; i++)
        {
            command += F("(");
            command += _parameters[i].name;
            command += F(":");
            command += String(*_parameters[i].value);
            if (i < parameters_number - 1)
            {
                command += F(",");
            }
        }
        Serial.println(command);
        return command;
    };

    uint8_t get_parameters_number()
    {
        return parameters_number;
    }

private:
    Parameter _parameters[parameters_number];
    uint8_t _parameter_count;
    const char *_name;
};

template <uint8_t plots_number>
class SerialPlotter
{
public:
    SerialPlotter() = default;

    ~SerialPlotter()
    {
        for (uint8_t i = 0; i < _plots_count; i++)
        {
            delete static_cast<BasePlot *>(_plots[i]);
        }
    };

    template <uint8_t parameters_number>
    Plot<parameters_number> &add_plot(const char *name)
    {
        static_assert(parameters_number > 0, "Plot must have at least one parameter");
        Plot<parameters_number> *new_plot = new Plot<parameters_number>(name);
        _plots[_plots_count] = new_plot;
        return *static_cast<Plot<parameters_number> *>(_plots[_plots_count++]);
    };

    void plot()
    {
        String command = F("{");
        command.reserve(64);
        for (uint8_t i = 0; i < plots_number; i++)
        {
            command += *(_plots[i]).generate_command();
        }
        command += F("}");

        Serial.print(command);
    }

private:
    void *_plots[plots_number];
    uint8_t _plots_count;
};
