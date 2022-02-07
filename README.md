# f28379d_cpu01_base

Codigo base para operação single core com f28379d.

Recursos utilizados:

- CPU01:
	- ePWM: 10kHz 2-level 3-pahse VSI PWM (dispara o SOC do ADC)
	- ADC: 20kHz, 13 channels (interrupção de fim de conversão [código do controle aqui])
	- PPB: overcurrent trip (interrupção compáração conversão ADC)
	- CLA: park transform (disparado por software)
	- SCI: nextion communication (interrupção de recebimento, transmissao no tempo oscioso do loop infinito)
  
### Hardware:
- ControlCard28379D-GEPAI-V1.0 + LAUNCHXL-F28379D
- NX8048K070_11
	
### Pinagem:
- UART: 8bits, no parity, 1 stop, 250 kbp
         
	| perif   | GPIOxxx |   cpu   | dir| hmi     | connector      |
	|---------|---------|---------|----|---------|----------------| 
	| SCIC_RX | 139     | f28379d | <- | nextion | J4TX (azul)    |
	| SCIC_TX | 056     | f28379d | -> | nextion | J4RX (amarelo) |
  
- PWM: SPWM, triangular carrier(10kHz)
	| phase | sup/inf | GPIOxxx | PWMxy |
	|-------|---------|---------|-------|
	| A     | ↑       | 000     | 1A    |
	| A     | ↓       | 001     | 1B    |
	| B     | ↑       | 002     | 2A    |
	| B     | ↓       | 003     | 2B    |
	| C     | ↑       | 004     | 3A    |
	| C     | ↓       | 005     | 3B    |
	
- ADC: 20 kHz samppling por segundo, 13 channels
	| signal | ADC result                | ADCINxx  | GPIOxxx | conector |
	|--------|---------------------------|----------|---------|----------|
	| iu     | AdcaResultRegs.ADCRESULT0 | a3	| 026 	  | JA2      |
	| iv     | AdcbResultRegs.ADCRESULT0 | b3 	| 025 	  | JA2      |
	| v0n    | AdccResultRegs.ADCRESULT3 | 14 	| 023 	  | JA2      |
	| vp0    | AdccResultRegs.ADCRESULT3 | c3 	| 024 	  | JA2      |
	| ic1    | AdcaResultRegs.ADCRESULT1 | a2 	| 029 	  | JA1      |
	| ib1    | AdccResultRegs.ADCRESULT1 | c5 	| 025 	  | JA1      |
	| vts1   | AdccResultRegs.ADCRESULT2 | c2 	| 027 	  | JA1      |
	| vrs1   | AdcbResultRegs.ADCRESULT2 | b2 	| 028 	  | JA1      |
	| ic2    | AdccResultRegs.ADCRESULT4 | 15 	| 063 	  | JA1      |
	| ib2    | AdcbResultRegs.ADCRESULT4 | b5 	| 065 	  | JA1      |
	| vts2   | AdcaResultRegs.ADCRESULT5 | a5 	| 066 	  | JA1      |
	| vrs2   | AdccResultRegs.ADCRESULT5 | c4 	| 067 	  | JA1      |      
	| pot    | AdcaResultRegs.ADCRESULT6 | a0 	| 030 	  | JA3      |
	
