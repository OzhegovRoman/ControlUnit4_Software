import QtQuick 2.4
import QtQuick.Controls 2.0

ListModel {
    id: settingsModel
    Component.onCompleted: {
        append({
                   name: "current",
                   itemText: "Current (uA)",
                   value: sspdCurrent,
                   type: "changable",
                   fixed: 1,
                   group: "Bias",
                   step: 0.1
               })
        append({
                   name: "voltage",
                   itemText: "Voltage (mV)",
                   value: sspdVoltage,
                   type: "unchangable",
                   fixed: 2,
                   group: "Bias"
               })
        append({
                   name: "short",
                   itemText: "Shorted",
                   value: sspdShorted ? 1: 0,
                   type: "switch",
                   group: "Bias"
               })
        append({
                   name: "amplifier",
                   itemText: "Amplifier",
                   value: sspdAmplifierTurnedOn,
                   type: "switch",
                   group: "Signal"
               })
        append({
                   name: "cmp",
                   itemText: "Comparator (mV)",
                   value: sspdCmpRefLevel,
                   type: "changable",
                   fixed: 0,
                   group: "Signal",
                   step: 10
               })
        append({
                   name: "cmp_on",
                   itemText: "Comparator",
                   value: sspdComparatorTurnedOn,
                   type: "switch",
                   group: "Signal"
               })
        append({
                   name: "counter",
                   itemText: "Counts",
                   value: sspdCounts,
                   type: "unchangable",
                   fixed: 0,
                   group: "Counter"
               })
        append({
                   name: "counter_timeOut",
                   itemText: "TimeOut (sec)",
                   value: sspdCounterTimeConst,
                   type: "changable",
                   fixed: 2,
                   group: "Counter",
                   step: 0.1
               })
        append({
                   name: "autoreset_on",
                   itemText: "Autoreset",
                   value: sspdAutoResetTurnedOn,
                   type: "switch",
                   group: "Autoreset"
               })
        append({
                   name: "threshold",
                   itemText: "Threshold (V)",
                   value: sspdAutoResetThreshold,
                   type: "changable",
                   fixed: 2,
                   group: "Autoreset",
                   step: 0.1

               })
        append({
                   name: "timeOut",
                   itemText: "TimeOut (sec)",
                   value: sspdAutoResetTimeOut,
                   type: "changable",
                   fixed: 2,
                   group: "Autoreset",
                   step: 0.1
               })
    }
}
