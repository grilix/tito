# TITO

Tito is an experiment.

Envisioned as a library for building firmware for dumb(ish) IoT devices, trying not to use the
heap as hard as possible.

It will handle and abstract the connection to WiFi and MQTT, for publishing/subscribing to topics.

## Sensors

Sensors are expected to interface with values, these come in two types:

- `ValueSource<T>` - Provides values from various sources
- `ValueDestination<T>` - Allows setting values into places

`ValueSources<T>` returns an `std::optional<T>` value to take into account scenarios where
values may not be available (sensors not working or just not yet available.)

We'll normally get sensors from components to delegate reading/writing.

## Sensor topics

Topics are built as a tree, which can be used either to retrieve the topic as a string, or to
compare against a string.

The tree would normally be in the form of:

    - <namespace>
        - <component>
            - <sensor>
                - state
                - command

## Sensor reporting

We'll send readings to the MQTT broker using the reporters, which implement the `Reportable`.

## Sensor commands

We would have `Commandable` objects that can receive commands and perform some basic actions, like
switching a digital pin.

These will be grouped into a `CommandRouter` which will centralize distributing commands over them.

# Tests

*TODO*

There are some **poor** tests (work in progress,) that can be run with:


    make test
