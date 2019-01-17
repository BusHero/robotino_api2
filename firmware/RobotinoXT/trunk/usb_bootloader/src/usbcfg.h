/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBCFG.H
 *      Purpose: USB Custom Configuration
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on Philips LPC2xxx microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBCFG_H__
#define __USBCFG_H__


//*** <<< Use Configuration Wizard in Context Menu >>> ***


/*
// <h> USB Configuration
//   <o0> Vendor ID <0x0000-0xFFFF>
//   <o1> Product ID <0x0000-0xFFFF>
//   <o2> Device Release <0x0000-0xFFFF>
//   <o3> USB Power
//        <i> Default Power Setting
//        <0=> Bus-powered
//        <1=> Self-powered
//   <o4> Port Selection (set to Port 1, except for LPC2378/87)
//        <1=> Port 1
//        <2=> Port 2
//   <q5> Use SoftConnect
//   <o6> Max Number of Interfaces <1-256>
//   <o7> Max Number of Endpoints  <1-32>
//   <o8> Max Endpoint 0 Packet Size
//        <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes
//   <e9> DMA Transfer
//     <i> Use DMA for selected Endpoints
//     <o9.0>  Endpoint 0 Out
//     <o9.1>  Endpoint 0 In
//     <o9.2>  Endpoint 1 Out
//     <o9.3>  Endpoint 1 In
//     <o9.4>  Endpoint 2 Out
//     <o9.5>  Endpoint 2 In
//     <o9.6>  Endpoint 3 Out
//     <o9.7>  Endpoint 3 In
//     <o9.8>  Endpoint 4 Out
//     <o9.9>  Endpoint 4 In
//     <o9.10> Endpoint 5 Out
//     <o9.11> Endpoint 5 In
//     <o9.12> Endpoint 6 Out
//     <o9.13> Endpoint 6 In
//     <o9.14> Endpoint 7 Out
//     <o9.15> Endpoint 7 In
//     <o9.16> Endpoint 8 Out
//     <o9.17> Endpoint 8 In
//     <o9.18> Endpoint 9 Out
//     <o9.19> Endpoint 9 In
//     <o9.20> Endpoint 10 Out
//     <o9.21> Endpoint 10 In
//     <o9.22> Endpoint 11 Out
//     <o9.23> Endpoint 11 In
//     <o9.24> Endpoint 12 Out
//     <o9.25> Endpoint 12 In
//     <o9.26> Endpoint 13 Out
//     <o9.27> Endpoint 13 In
//     <o9.28> Endpoint 14 Out
//     <o9.29> Endpoint 14 In
//     <o9.30> Endpoint 15 Out
//     <o9.31> Endpoint 15 In
//   </e>
// </h>
*/

#define VENDOR_ID			0x1E29
#define PRODUCT_ID			0x040C
#define DEVICE_RELEASE		0x0110
#define USB_POWER           1
#define USB_PORTSEL         1
#define SOFTCONNECT         1
#define USB_IF_NUM          4
#define USB_EP_NUM          32
#define USB_MAX_PACKET0     64
#define USB_DMA             0
#define USB_DMA_EP          0x00000000


/*
// <h> USB Event Handlers
//   <h> Device Events
//     <o0.0> Power Event
//     <o1.0> Reset Event
//     <o2.0> Suspend Event
//     <o3.0> Resume Event
//     <o4.0> Remote Wakeup Event
//     <o5.0> Start of Frame Event
//     <o6.0> Error Event
//   </h>
//   <h> Endpoint Events
//     <o7.0>  Endpoint 0 Event
//     <o7.1>  Endpoint 1 Event
//     <o7.2>  Endpoint 2 Event
//     <o7.3>  Endpoint 3 Event
//     <o7.4>  Endpoint 4 Event
//     <o7.5>  Endpoint 5 Event
//     <o7.6>  Endpoint 6 Event
//     <o7.7>  Endpoint 7 Event
//     <o7.8>  Endpoint 8 Event
//     <o7.9>  Endpoint 9 Event
//     <o7.10> Endpoint 10 Event
//     <o7.11> Endpoint 11 Event
//     <o7.12> Endpoint 12 Event
//     <o7.13> Endpoint 13 Event
//     <o7.14> Endpoint 14 Event
//     <o7.15> Endpoint 15 Event
//   </h>
//   <h> USB Core Events
//     <o8.0>  Set Configuration Event
//     <o9.0>  Set Interface Event
//     <o10.0> Set/Clear Feature Event
//   </h>
// </h>
*/

#define USB_POWER_EVENT     0
#define USB_RESET_EVENT     1
#define USB_SUSPEND_EVENT   1
#define USB_RESUME_EVENT    1
#define USB_WAKEUP_EVENT    0
#define USB_SOF_EVENT       0
#define USB_ERROR_EVENT     0
#define USB_EP_EVENT        0x0005
#define USB_CONFIGURE_EVENT 1
#define USB_INTERFACE_EVENT 0
#define USB_FEATURE_EVENT   0


/*
// <e0> USB Class Support
//   <e1> Human Interface Device (HID)
//     <o2> Interface Number <0-255>
//   </e>
//   <e3> Mass Storage
//     <o4> Interface Number <0-255>
//   </e>
//   <e5> Audio Device
//     <o6> Control Interface Number <0-255>
//     <o7> Streaming Interface 1 Number <0-255>
//     <o8> Streaming Interface 2 Number <0-255>
//   </e>
// </e>
*/

#define USB_CLASS           1
#define USB_HID             0
#define USB_HID_IF_NUM      0
#define USB_MSC             1
#define USB_MSC_IF_NUM      0
#define USB_AUDIO           0
#define USB_ADC_CIF_NUM     0
#define USB_ADC_SIF1_NUM    1
#define USB_ADC_SIF2_NUM    2


#endif  /* __USBCFG_H__ */
