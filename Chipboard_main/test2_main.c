/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "stm32f4_discovery.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_gpio_ex.h"
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "NetworkInterface.h"
#include "task.h"
#include "FreeRTOS_Sockets.h"

#include "simpleI2Cdriver.h"

/*Handles*/

static RNG_HandleTypeDef rngHandle;
/**********/

int warning ;

void vConfigureTimerForRunTimeStats( void ){

	/*Enable clock to TIM2*/
	RCC->APB1ENR |= (1 << 0);

	/*Set prescaler to 2048*/
	TIM2->PSC |= (1 << 11);

	/*Enable TIM2*/
	TIM2->CR1 |= (1 << 0);
}

void systemSetup(){
	SystemInit();
	/*Initialize HAL*/
	HAL_Init();

	/*Setup system clock*/
	HAL_RCC_DeInit();

	/*Configure oscillator*/
	RCC_OscInitTypeDef oscInit;
	oscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscInit.HSEState = RCC_HSE_ON;
	oscInit.HSIState = RCC_HSI_OFF;
	oscInit.PLL.PLLState = RCC_PLL_ON;
	oscInit.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscInit.PLL.PLLM = 8;
	oscInit.PLL.PLLN = 336;
	oscInit.PLL.PLLP = 2;
	oscInit.PLL.PLLQ = 7;

	HAL_RCC_OscConfig(&oscInit);

	/*Configure clock*/
	RCC_ClkInitTypeDef clkInit;
	clkInit.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkInit.APB1CLKDivider = RCC_SYSCLK_DIV4;
	clkInit.APB2CLKDivider = RCC_SYSCLK_DIV2;

	HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_5);
	SystemCoreClockUpdate();
}

void setupLED(){

	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	/*Setup pins for LEDs*/

	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth){
	/*Setup ethernet*/

	/*Enable Ethernet interrupt*/
	HAL_NVIC_SetPriority(ETH_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0);

	/*Enable ethernet clocks*/
	__HAL_RCC_ETHMAC_CLK_ENABLE();
	__HAL_RCC_ETHMACTX_CLK_ENABLE();
	__HAL_RCC_ETHMACRX_CLK_ENABLE();

	/*Enable RCC clocks for GPIO pins*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	/*Setup pins in port A*/
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;		//Route the pins to MAC peripheral
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Setup pins in port B*/
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Setup pins in port C*/
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;		//Route the pins to MAC peripheral
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* The MAC address array is not declared const as the MAC address will
normally be read from an EEPROM and not hard coded (in real deployed
applications).*/
uint8_t ucMACAddress[ 6 ] = { 0xba, 0xd0, 0xba, 0xd0, 0xba, 0xbe };

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */

static const uint8_t ucIPAddress[ 4 ] = { 192, 168, 0, 254 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 0, 1 };


/* The following is the address of an OpenDNS server. */
static const uint8_t ucDNSServerAddress[ 4 ] = { 208, 67, 222, 222 };

void prvProcessData(char *cRxedData){
	const char led1On[] = "LED1 ON";
	const char led2On[] = "LED2 ON";
	const char led3On[] = "LED3 ON";
	const char led4On[] = "LED4 ON";

	const char led1Off[] = "LED1 OFF";
	const char led2Off[] = "LED2 OFF";
	const char led3Off[] = "LED3 OFF";
	const char led4Off[] = "LED4 OFF";

	const char* commands[] = {led1On, led2On, led3On, led4On, led1Off, led2Off, led3Off, led4Off};

	/*Strip /r and /n*/
	int done = 0;
	int i = 0;
	while(done != 1){
		if(cRxedData[i] == '\r'){
			/* Clear \n */
			cRxedData[i+1] = 0;
			/* Clear \r */
			cRxedData[i] = 0;

			done = 1;
		}
		i++;
	}

	int whichCommand;
	int commandFound = 0;
	for (whichCommand = 0; whichCommand < sizeof(commands)/sizeof(commands[0]); whichCommand++){
		if(strcmp(commands[whichCommand], cRxedData) == 0){
			commandFound = 1;
			break;
		}
	}

	if(commandFound == 1){
		switch(whichCommand){
		case 0: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		break;

		case 1: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
		break;

		case 2: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
		break;

		case 3: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);
		break;

		case 4: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
		break;

		case 5: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
		break;

		case 6: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
		break;

		case 7: HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
		break;
		}
	}
}

#define BUFFER_SIZE 512
static void prvServerConnectionInstance (void *pvParameters){

	Socket_t xSocket;
	static char cRxedData[ BUFFER_SIZE ];
	BaseType_t lBytesReceived;

	    /* It is assumed the socket has already been created and connected before
	    being passed into this RTOS task using the RTOS task's parameter. */
	    xSocket = ( Socket_t ) pvParameters;

	    for( ;; )
	    {
	        /* Receive another block of data into the cRxedData buffer. */
	        lBytesReceived = FreeRTOS_recv( xSocket, &cRxedData, BUFFER_SIZE, 0 );

	        if( lBytesReceived > 0 )
	        {
	            /* Data was received, process it here. */
	            prvProcessData( cRxedData );
	        }
	        else if( lBytesReceived == 0 )
	        {
	            /* No data was received, but FreeRTOS_recv() did not return an error.
	            Timeout? */
	        }
	        else
	        {
	            /* Error (maybe the connected socket already shut down the socket?).
	            Attempt graceful shutdown. */
	            FreeRTOS_shutdown( xSocket, FREERTOS_SHUT_RDWR );
	            break;
	        }
	    }

	    vTaskDelete( NULL );
}

static void vCreateTCPServerSocket( void *pvParameters )
{
struct freertos_sockaddr xClient, xBindAddress;
Socket_t xListeningSocket, xConnectedSocket;
socklen_t xSize = sizeof( xClient );
static const TickType_t xReceiveTimeOut = portMAX_DELAY;
const BaseType_t xBacklog = 20;

    /* Attempt to open the socket. */
    xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                        FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
                                        FREERTOS_IPPROTO_TCP );

    /* Check the socket was created. */
    configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );

    /* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    FreeRTOS_setsockopt() to change the buffer sizes from their default then do
    it here!.  (see the FreeRTOS_setsockopt() documentation. */

    /* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
    be used with FreeRTOS_setsockopt() to change the sliding window size from
    its default then do it here! (see the FreeRTOS_setsockopt()
    documentation. */

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt( xListeningSocket,
                         0,
                         FREERTOS_SO_RCVTIMEO,
                         &xReceiveTimeOut,
                         sizeof( xReceiveTimeOut ) );

    /* Set the listening port to 0x1111. */
    xBindAddress.sin_port = FreeRTOS_htons( 0x1111 );

    /* Bind the socket to the port that the client RTOS task will send to. */
    FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );

    /* Set the socket into a listening state so it can accept connections.
    The maximum number of simultaneous connections is limited to 20. */
    FreeRTOS_listen( xListeningSocket, xBacklog );

    for( ;; )
    {
        /* Wait for incoming connections. */
        xConnectedSocket = FreeRTOS_accept( xListeningSocket, &xClient, &xSize );
        configASSERT( xConnectedSocket != FREERTOS_INVALID_SOCKET );

        /* Spawn a RTOS task to handle the connection. */
        xTaskCreate( prvServerConnectionInstance,
                     "EchoServer",
                     configMINIMAL_STACK_SIZE*2,
                     ( void * ) xConnectedSocket,
                     tskIDLE_PRIORITY,
                     NULL );
    }
}

void vTCPSend(void *socket, char *pcBufferToTransmit, const size_t xTotalLengthToSend)
{
	Socket_t xSocket = (Socket_t) socket;
	BaseType_t xAlreadyTransmitted = 0, xBytesSent = 0;
	size_t xLenToSend;

    /* Connect to the remote socket.  The socket has not previously been bound to
    a local port number so will get automatically bound to a local port inside
    the FreeRTOS_connect() function. */

        /* Keep sending until the entire buffer has been sent. */

        while( xAlreadyTransmitted < xTotalLengthToSend )
        {
            /* How many bytes are left to send? */
            xLenToSend = xTotalLengthToSend - xAlreadyTransmitted;
            xBytesSent = FreeRTOS_send( /* The socket being sent to. */
                                        xSocket,
                                        /* The data being sent. */
                                        &( pcBufferToTransmit[ xAlreadyTransmitted ] ),
                                        /* The remaining length of data to send. */
                                        xLenToSend,
                                        /* ulFlags. */
                                        0 );

            if( xBytesSent >= 0 )
            {
                /* Data was sent successfully. */
                xAlreadyTransmitted += xBytesSent;

            }
            else
            {

            	/* Initiate graceful shutdown. */
				FreeRTOS_shutdown( xSocket, FREERTOS_SHUT_RDWR );

				/* Wait for the socket to disconnect gracefully (indicated by FreeRTOS_recv()
				returning a FREERTOS_EINVAL error) before closing the socket. */
				while( FreeRTOS_recv( xSocket, pcBufferToTransmit, xTotalLengthToSend, 0 ) >= 0 )
				{
					/* Wait for shutdown to complete.  If a receive block time is used then
					this delay will not be necessary as FreeRTOS_recv() will place the RTOS task
					into the Blocked state anyway. */
					vTaskDelay( configTICK_RATE_HZ/4 );

					/* Note - real applications should implement a timeout here, not just
					loop forever. */
				}

                break;
            }
        }
}


/*I'm sorry for this... started to run out of time... :DD*/
void turnSaunaOnOff(char *receivedData){
	if((receivedData[503] == '1') && (warning == 0)){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
	}else{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
	}
}

static void getSaunaOnOffData(void *pvParameters){

	Socket_t xSocket;

	warning = 0;

	struct freertos_sockaddr xRemoteAddress;

	/* Set the IP address and port  of the remote socket
	to which this client socket will transmit. */
	xRemoteAddress.sin_port = FreeRTOS_htons( 80 );
	xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick( 195, 148, 105, 101 );

	/*xRemoteAddress.sin_port = FreeRTOS_htons( 15000 );
	xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick( 192, 168, 1, 1 );*/

	BaseType_t lBytesReceived = 0;
	static char rxData[BUFFER_SIZE];

	while(1){

		/* Create a socket. */
		xSocket = FreeRTOS_socket( FREERTOS_AF_INET,
								   FREERTOS_SOCK_STREAM,/* FREERTOS_SOCK_STREAM for TCP. */
								   FREERTOS_IPPROTO_TCP );
		configASSERT( xSocket != FREERTOS_INVALID_SOCKET );


		FreeRTOS_connect( xSocket, &xRemoteAddress, sizeof( xRemoteAddress ) );

		char dataBuffer[200] = {0};

		sprintf(dataBuffer, "GET /~hieuv/getAndroidData.php HTTP/1.1"
				"\r\nHost: users.metropolia.fi"
				"\r\nContent-Length: 0"
				"\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");

		vTCPSend(xSocket, dataBuffer, sizeof(dataBuffer)/sizeof(dataBuffer[0]));

		/*Wait for HTTP 200 OK status from server*/
		while(lBytesReceived <= 0){
			lBytesReceived = FreeRTOS_recv( xSocket, &rxData, BUFFER_SIZE, 0 );
		}

		lBytesReceived = 0;

		turnSaunaOnOff(rxData);

		vTaskDelay(configTICK_RATE_HZ*2);

		FreeRTOS_closesocket( xSocket );


	}
}


static void sendThermometerData(void *pvParameters){

	Socket_t xSocket;

	struct freertos_sockaddr xRemoteAddress;

	/* Set the IP address and port  of the remote socket
	to which this client socket will transmit. */
	xRemoteAddress.sin_port = FreeRTOS_htons( 80 );
	xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick( 195, 148, 105, 101 );

	/*xRemoteAddress.sin_port = FreeRTOS_htons( 15000 );
	xRemoteAddress.sin_addr = FreeRTOS_inet_addr_quick( 192, 168, 1, 1 );*/


	int temperature = 20;//0;
	int humidity = 10; //0;

	//BaseType_t lBytesReceived = 0;
	//static char cRxedData[BUFFER_SIZE];
	while(1){

		//SDAbusErrorCheck();
		//measurementRequest();
		//dataFetch();

		int t = rand()% 4 + (-1);
		int h = rand()% 4 + (-1);

		//humidity = getHumidityData();
		//temperature = getTemperatureData();

		/* Create a socket. */
		xSocket = FreeRTOS_socket( FREERTOS_AF_INET,
								   FREERTOS_SOCK_STREAM,/* FREERTOS_SOCK_STREAM for TCP. */
								   FREERTOS_IPPROTO_TCP );
		configASSERT( xSocket != FREERTOS_INVALID_SOCKET );


		FreeRTOS_connect( xSocket, &xRemoteAddress, sizeof( xRemoteAddress ) );

		char dataBuffer[200] = {0};

		sprintf(dataBuffer, "POST /~hieuv/addTemp.php HTTP/1.1"
				"\r\nHost: users.metropolia.fi"
				"\r\nContent-Length: 20"
				"\r\nContent-Type: application/x-www-form-urlencoded"
				"\r\n\r\ntemp1=%d&hum1=%d", temperature, humidity);

		/*sprintf(dataBuffer, "GET /~hieuv/getAndroidData.php HTTP/1.1"
				"\r\nHost: users.metropolia.fi"
				"\r\nContent-Length: 0"
				"\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n");*/

		vTCPSend(xSocket, dataBuffer, sizeof(dataBuffer)/sizeof(dataBuffer[0]));

		/*Wait for HTTP 200 OK status from server*/
		/*while(lBytesReceived <= 0){
			lBytesReceived = FreeRTOS_recv( xSocket, &cRxedData, BUFFER_SIZE, 0 );
		}*/

		vTaskDelay(configTICK_RATE_HZ);

		/* The socket has shut down and is safe to close. */
		FreeRTOS_closesocket( xSocket );

		temperature += t;
		humidity += h;

		if( temperature > 90 ) { warning = 1;} else { warning = 0; }

		if( temperature > 140 ) { temperature = 20;}
	}
}


void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
    	HAL_NVIC_EnableIRQ(ETH_IRQn);
    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);

        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+TCP can be created here
             * to ensure they are not created before the network is usable.
             */

        	xTaskCreate(vCreateTCPServerSocket, "tcpListeningSocket", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        	xTaskCreate(sendThermometerData, "tcpClientSocket1", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
        	xTaskCreate(getSaunaOnOffData, "tcpClientSocket2", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
            xTasksAlreadyCreated = pdTRUE;
        }
    }
}

void vApplicationPingReplyHook (ePingReplyStatus_t eStatus, uint16_t usIdentifier){
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
}

BaseType_t xApplicationDNSQueryHook( const char *pcName ){
	return 0;
}

/*Enable clock to random number generator*/
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng){
	__HAL_RCC_RNG_CLK_ENABLE();
}

/*Disable clock to random number generator*/
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng){
	__HAL_RCC_RNG_CLK_DISABLE();
}

/*For DHCP transaction ID and client TCP port generation*/
UBaseType_t uxRand(){

	static uint32_t randVal;

	rngHandle.Instance = RNG;
	/*Previous random number*/
	rngHandle.RandomNumber = randVal;

	HAL_RNG_Init(&rngHandle);

	HAL_RNG_GenerateRandomNumber(&rngHandle, &randVal);

	HAL_RNG_DeInit(&rngHandle);

	uint32_t transactionID = randVal % 49000 + 1024;

	return transactionID;
}

int main(void)
{
	systemSetup();
	setupLED();

	setupI2CGPIO();
	setupI2C();

 /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
	are created in the vApplicationIPNetworkEventHook() hook function
	below.  The hook function is called when the network connects. */

	FreeRTOS_IPInit( ucIPAddress,
					 ucNetMask,
					 ucGatewayAddress,
					 ucDNSServerAddress,
					 ucMACAddress );

	/*
	 * Other RTOS tasks can be created here.
	 */

	/* Start the RTOS scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks to be created. */
	for( ;; );
}
