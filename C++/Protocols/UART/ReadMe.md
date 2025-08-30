# Introduction

This document is created for Uart protocol information.

## Summary

- UART protol is  an asychronous receiver/transmitter protocol
- Simple two wire protocol for exchanging serial data
- No shared clock, baud rate must be configured the same on both sides
- Start/stop bits used to frame user data
- Optionaly parity bit for detecting single-bit errors
- Widely used serial data protocol, but slowly being replaced by the SPI, I2C, USB and Ethernet
- important for lower-speed, low-throughput applications. Because of low cost and easer to implement

## Basics

- TX to RX wires
- Can be simplex, half-duplex, or full-duplex
- Data is transmitted as frames

## UART Frame

- UART frame consists of:
  - Start/Stop bits
  - Data bits
  - Parity bit (optional)
- High voltage ("mark")=1, low voltage("space")=0 and UART doesn't specify voltage range
- In the idle state, the line is held high. (Ease of detection of damaged lines)
