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
                   text: "0.0",
                   fixed: 1,
                   group: "Bias",
                   step: 0.1
               })
        append({
                   name: "voltage",
                   itemText: "Voltage (mV)",
                   value: sspdVoltage,
                   type: "unchangable",
                   text: "0.00",
                   fixed: 2,
                   group: "Bias"
               })
        append({
                   name: "short",
                   itemText: "Shorted",
                   value: sspdShorted ? 1: 0,
                   type: "switch",
                   text: "",
                   group: "Bias"
               })
        append({
                   name: "amplifier",
                   itemText: "Amplifier",
                   value: sspdAmplifierTurnedOn,
                   type: "switch",
                   text: "",
                   group: "Signal"
               })
        append({
                   name: "cmp",
                   itemText: "Comparator (mV)",
                   value: sspdCmpRefLevel,
                   type: "changable",
                   text: "0",
                   fixed: 0,
                   group: "Signal",
                   step: 10
               })
        append({
                   name: "cmp_on",
                   itemText: "Comparator",
                   value: sspdComparatorTurnedOn,
                   type: "switch",
                   text: "",
                   group: "Signal"
               })
        append({
                   name: "counter",
                   itemText: "Counts",
                   value: sspdCounts,
                   type: "unchangable",
                   text: "0",
                   fixed: 0,
                   group: "Counter"
               })
        append({
                   name: "counter_timeOut",
                   itemText: "TimeOut (sec)",
                   value: sspdCounterTimeConst,
                   type: "changable",
                   text: "0.00",
                   fixed: 2,
                   group: "Counter",
                   step: 0.1
               })
        append({
                   name: "autoreset_on",
                   itemText: "Autoreset",
                   value: sspdAutoResetTurnedOn,
                   type: "switch",
                   text: "",
                   group: "Autoreset"
               })
        append({
                   name: "threshold",
                   itemText: "Threshold (V)",
                   value: sspdAutoResetThreshold,
                   type: "changable",
                   text: "0.00",
                   fixed: 2,
                   group: "Autoreset",
                   step: 0.1

               })
        append({
                   name: "timeOut",
                   itemText: "TimeOut (sec)",
                   value: sspdAutoResetTimeOut,
                   type: "changable",
                   text: "0.00",
                   fixed: 2,
                   group: "Autoreset",
                   step: 0.1
               })
    }
}
