----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:37:22 01/19/2009 
-- Design Name: 
-- Module Name:    ad7490_ctrl - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ea09_ctrl is
    Port ( 
			-- external 50 Mhz Oscillator
			CLK_IN			: in 		STD_LOGIC;
			  
			-- EMC-Interface-Signals (to LPC2377)
			CE_N 			: in		STD_LOGIC;
			OE_N 			: in		STD_LOGIC;
			WE_N 			: in		STD_LOGIC;
			IO 			: inout		STD_LOGIC_VECTOR(7 downto 0);
			A 				: in		STD_LOGIC_VECTOR(3 downto 0);
			  
			-- Motor: Encoder
			ENC_A			: in		STD_LOGIC_VECTOR(3 downto 0);
			ENC_B			: in		STD_LOGIC_VECTOR(3 downto 0);
			
			THFLG			: in		STD_LOGIC_VECTOR(3 downto 0);

			-- Digital IOs
			DI				: in 		STD_LOGIC_VECTOR(7 downto 0);
			RELAY			: out		STD_LOGIC_VECTOR(1 downto 0);
			  
			-- Extenders
			-- NOTE: IO_A(7) is not used in this design
			--IO_A			: inout		STD_LOGIC_VECTOR(6 downto 0);
			PC_INIT		: inout		STD_LOGIC;
			PC_STROBE	: inout		STD_LOGIC;
			PC_ERR		: inout		STD_LOGIC;
			SLCT			: inout		STD_LOGIC;
			PE				: inout		STD_LOGIC;
			BUSY			: inout		STD_LOGIC;
			ACK			: inout		STD_LOGIC;
			
			IO_B			: inout		STD_LOGIC_VECTOR(7 downto 0);
			
			SPI1_E_SCK	: inout		STD_LOGIC;
			SPI1_E_MISO	: inout		STD_LOGIC;
			SPI1_E_SS	: inout		STD_LOGIC;
			SPI1_E_MOSI	: inout		STD_LOGIC;
			  
			-- LEDs
			LED			: out		STD_LOGIC_VECTOR(2 downto 0);
			ALIVE_LED	: out		STD_LOGIC;
			  
			POWER_ON		: out		STD_LOGIC
			);
end ea09_ctrl;


-- NOTE: Uses a lock signal instead of a buffer
architecture Behavioral of ea09_ctrl is

-- CLK and reset signals
signal CLK			: STD_LOGIC;
signal RST_N 		: STD_LOGIC;
signal locked		: STD_LOGIC;
signal rst_cnt		: INTEGER range 0 to 255;

begin

-- 48MHz CLK signal
clock : entity work.ea09dcm(Behavioral)
port map(
	CLKIN_IN => CLK_IN,
	RST_IN => '0',
	CLKIN_IBUFG_OUT => CLK,
	CLK0_OUT => open,
	LOCKED_OUT => locked
);

reset : process( CLK ) 
begin
if( rising_edge( CLK ) ) then
	if( locked = '0' ) then
		RST_N <= '0';
		rst_cnt <= 0;
	else
		-- reset all entities when clock is stable
		if( rst_cnt < 100 ) then
			rst_cnt <= rst_cnt + 1;
		else
			RST_N <= '1';
		end if;
	end if;
end if;
end process;

--  Registerbeschreibung
--  Reg  Bit  Beschreibung            Richtung
--  0    0-7  motor 0 up counter      FPGA->LPC
--  1    0-7  motor 0 down counter    FPGA->LPC
--  2    0-7  motor 1 up counter      FPGA->LPC
--  3    0-7  motor 1 down counter    FPGA->LPC
--  4    0-7  motor 2 up counter      FPGA->LPC
--  5    0-7  motor 2 down counter    FPGA->LPC
--  6    0-7  motor 3 up counter      FPGA->LPC
--  7    0-7  motor 3 down counter    FPGA->LPC
--  8    0    LED 0                   LPC->FPGA
--       1    LED 1                   LPC->FPGA
--       2    LED 2                   LPC->FPGA
--       3    THFLG 0                 FPGA->LPC
--       4    THFLG 1                 FPGA->LPC
--       5    THFLG 2                 FPGA->LPC
--       6    THFLG 3                 FPGA->LPC
--       3    (LED 3) jetzt ALIVE     LPC->FPGA
--       4    Relay 0                 LPC->FPGA
--       5    Relay 1                 LPC->FPGA
--       6    Power on                LPC->FPGA
--  9    0-7  (Digitaler Ausgang)     LPC->FPGA
--  10   0-7  Digitaler Eingang       FPGA->LPC
--  11   0    ACK                     LPC<->FPGA
--       1    BUSY                    LPC<->FPGA
--       2    PE                      LPC<->FPGA
--       3    SLCT                    LPC<->FPGA
--       4    PC_ERR                  LPC<->FPGA
--       5    PC_STROBE               LPC<->FPGA
--       6    PC_INIT                 LPC<->FPGA
--       7    ALIVE  (immer 0)        LPC<-FPGA
--  12   0-7  Erweiterung IO_B        LPC<->FPGA
--  13   0-7  Konfiguration IO_A      LPC->FPGA
--  14   0-7  Konfiguration IO_B      LPC->FPGA

set_registers : process( CLK ) is
	-- Internal CE signal:
	-- The address presented @ A is valid 0.20ns after the falling edge of CE_N!
	variable ce_n_int	: STD_LOGIC;
	-- Internal address-valid signal
	variable emc_state	: INTEGER range 0 to 4;
	
	-- INDEXES is defined from 0 to #(Registers+1) -->
	subtype INDEXES is INTEGER range 0 to 15;
	variable index		: INDEXES;

	variable io_a_config : STD_LOGIC_VECTOR(7 downto 0);
	variable io_b_config : STD_LOGIC_VECTOR(7 downto 0);

	-- counter to toggle LED(7)
	variable alive_led_count	: INTEGER RANGE 0 to 50000000;
	variable alive_led_state   : STD_LOGIC;

	-- A,B werden nur mit 5MHz abgetastet
	variable enc_filter_count	: INTEGER RANGE 0 to 10 := 0;
	
	subtype ENC_FILTER_T is INTEGER range 0 to 3;
	type ENC_FILTER_VEC is array (0 to 3) of ENC_FILTER_T;

	variable enc_filter_a_high : ENC_FILTER_VEC;
	variable enc_filter_a_low : ENC_FILTER_VEC;
	variable enc_filter_b_high : ENC_FILTER_VEC;
	variable enc_filter_b_low : ENC_FILTER_VEC;
	
	variable enc_a_filtered : STD_LOGIC_VECTOR(3 downto 0) ;
	variable enc_b_filtered : STD_LOGIC_VECTOR(3 downto 0);
	
	-- motor encoders
	type ENC_BIT_T is array (0 to 3) of STD_LOGIC;
	type ENC_CNT_T is array (0 to 3) of STD_LOGIC_VECTOR(7 downto 0);
	type ENC_STATE_T is ( A1, A0, B1, B0 );
	type ENC_STATE_VEC is array (0 to 3) of ENC_STATE_T;

	variable state : ENC_STATE_VEC;
	variable state_pre : ENC_STATE_VEC;
	variable p_a : ENC_BIT_T;
	variable p_b : ENC_BIT_T;
	variable a_pre : ENC_BIT_T;
	variable b_pre : ENC_BIT_T;
	variable up : ENC_CNT_T;
	variable down : ENC_CNT_T;
	
begin
if( rising_edge( CLK ) ) then
	if( RST_N = '0' ) then
		alive_led_count := 0;
		alive_led_state := '0';
		
		io_a_config := (others => '0'); --Voreinstellung ist Eingang
		io_b_config := (others => '0'); --Voreinstellung ist Eingang
		
		index := INDEXES'low;
		
		PC_INIT <= 'Z';
		PC_STROBE <= 'Z';
		PC_ERR <= 'Z';
		SLCT <= 'Z';
		PE <= 'Z';
		BUSY <= 'Z';
		ACK <= 'Z';

		IO_B <= ( others => 'Z' );
		IO <= ( others => 'Z' );
		RELAY <= ( others => '0' );
		
		SPI1_E_SCK <= 'Z';
		SPI1_E_MISO <= 'Z';
		SPI1_E_SS <= 'Z';
		SPI1_E_MOSI <= 'Z';
		
		LED <= (others => '1');
		ALIVE_LED <= '0';
		
		POWER_ON <= '0';
		
		ce_n_int := '1';
		emc_state := 0;
		
		for i in 0 to 3 loop
			state(i) := A0;
			state_pre(i) := A0;
			p_a(i) := '0';
			p_b(i) := '0';
			a_pre(i) := '0';
			b_pre(i) := '0';
			up(i) := (others => '0');
			down(i) := (others => '0');
			enc_filter_a_high(i) := 0;
			enc_filter_a_low(i) := 0;
			enc_filter_b_high(i) := 0;
			enc_filter_b_low(i) := 0;
			enc_a_filtered := (others => '0');
			enc_b_filtered := (others => '0');
		end loop;
		enc_filter_count := 0;
		
	else
		IO <= ( others => 'Z' );
		ce_n_int := CE_N;
		
		if( ce_n_int = '0' ) then 	
			---------------------
			-- fpga selected!
			---------------------
			case emc_state is
				-- 1 CLK cycle after CE_N went low:
				when 0 =>
					--LED(0) <= A(0);
					--LED(1) <= A(1);
					--LED(2) <= A(2);
					--LED(3) <= A(3);
					-- get current register address @ A if it exists
					if( A <= INDEXES'high-1 ) then
						index := to_integer( UNSIGNED( A ) );
					else
						index := INDEXES'low;
					end if;
					
					emc_state := 1;
				-- 2 CLK cycles after CE_N went low:
				when 1 =>
					emc_state := 2;

				-- 3 CLK cycles after CE_N went low:					
				when 2 =>
					emc_state := 3;
					
				-- 4 CLK cycles after CE_N went low:
				when 3 =>
					case OE_N is
						when '0' =>
							-- output data if OE_N is low!
							-- output latency <= 142ns after falling_edge( CE_N ) => 
							-- 	LPC2378: CLK(LPC2378) = ???
							-- ?	In  "Static Memory Read Delay Registers (EMCStaticWaitRd0-1 - 0xFFE0 820C, 22C)"
							--	?	Set "WAITRD" = ? 
							-- ?  Set "WAITOEN" = 0
							
							case index is
								when 0 =>
									IO <= up(0);
									up(0) := (others => '0');
									
								when 1 =>
									IO <= down(0);
									down(0) := (others => '0');
								
								when 2 =>
									IO <= up(1);
									up(1) := (others => '0');
								
								when 3 =>
									IO <= down(1);
									down(1) := (others => '0');
								
								when 4 =>
									IO <= up(2);
									up(2) := (others => '0');
								
								when 5 =>
									IO <= down(2);
									down(2) := (others => '0');
								
								when 6 =>
									IO <= up(3);
									up(3) := (others => '0');
								
								when 7 =>
									IO <= down(3);
									down(3) := (others => '0');
									
								when 8 =>
									IO(3) <= THFLG(0);
									IO(4) <= THFLG(1);
									IO(5) <= THFLG(2);
									IO(6) <= THFLG(3);
									
								when 10 =>
									IO <= DI;
									
								when 11 =>
									IO(0) <= ACK;
									IO(1) <= BUSY;
									IO(2) <= PE;
									IO(3) <= SLCT;
									IO(4) <= PC_ERR;
									IO(5) <= PC_STROBE;
									IO(6) <= PC_INIT;
									IO(7) <= '0';
									
								when 12 =>
									IO <= IO_B;
									
								when 13 =>
									IO <= io_a_config;
									
								when 14 =>
									IO <= io_a_config;
									
								when others =>
									IO <= ( others => '0' );
									
							end case;
							
							emc_state := 4;
								
						when '1' =>
							if( WE_N = '0' ) then
								case index is
									when 8 =>
										for i in 0 to 2 loop
											LED(i) <= IO(i);
										end loop;
										
										RELAY(0) <= IO(4);
										RELAY(1) <= IO(5);
										
										POWER_ON <= IO(6);
									
									when 9 =>
										--DO <= IO;
										
									when 11 =>
										for i in 0 to 6 loop
											case i is
												when 0 =>
													if( io_a_config(i) = '1' ) then
														ACK <= IO(i);
													else
														ACK <= 'Z';
													end if;
													
												when 1 =>
													if( io_a_config(i) = '1' ) then
														BUSY <= IO(i);
													else
														BUSY <= 'Z';
													end if;
													
												when 2 =>
													if( io_a_config(i) = '1' ) then
														PE <= IO(i);
													else
														PE <= 'Z';
													end if;
													
												when 3 =>
													if( io_a_config(i) = '1' ) then
														SLCT <= IO(i);
													else
														SLCT <= 'Z';
													end if;
													
												when 4 =>
													if( io_a_config(i) = '1' ) then
														PC_ERR <= IO(i);
													else
														PC_ERR <= 'Z';
													end if;
													
												when 5 =>
													if( io_a_config(i) = '1' ) then
														PC_STROBE <= IO(i);
													else
														PC_STROBE <= 'Z';
													end if;
													
												when 6 =>
													if( io_a_config(i) = '1' ) then
														PC_INIT <= IO(i);
													else
														PC_INIT <= 'Z';
													end if;
													
												when others => null;
											
											end case;
												
										end loop;
										
									when 12 =>
										for i in 0 to 7 loop
											if( io_b_config(i) = '1' ) then
												IO_B(i) <= IO(i);
											else
												IO_B(i) <= 'Z';
											end if;
										end loop;
										
									when 13 =>
										io_a_config := IO;
										
									when 14 =>
										io_b_config := IO;
										
									when others => null;
									
								end case;
							end if;
							
						when others => null;
						
					end case;
					
					when others => null;
					
			end case;
		else
			---------------------
			-- fpga unselected!
			---------------------	
			emc_state := 0;

		end if;
		
		if( alive_led_count < 50000000 ) then
			alive_led_count := alive_led_count + 1;
		else
			alive_led_count := 0;
			ALIVE_LED <= alive_led_state;
			alive_led_state := not alive_led_state;
		end if;
		
		if( enc_filter_count < 9 ) then
			enc_filter_count := enc_filter_count + 1;
		else
			enc_filter_count := 0;
		
			for i in 0 to 3 loop
				if( ENC_A(i) = '1' ) then
					if( enc_filter_a_high(i) < 3 ) then
						enc_filter_a_high(i) := enc_filter_a_high(i) + 1;
					end if;
					enc_filter_a_low(i) := 0;
				end if;
				
				if( enc_filter_a_high(i) = 3 ) then
					enc_a_filtered(i) := '1';
				end if;
				
				if( ENC_A(i) = '0' ) then
					if( enc_filter_a_low(i) < 3 ) then
						enc_filter_a_low(i) := enc_filter_a_low(i) + 1;
					end if;
					enc_filter_a_high(i) := 0;
				end if;
				
				if( enc_filter_a_low(i) = 3 ) then
					enc_a_filtered(i) := '0';
				end if;
				
				if( ENC_B(i) = '1' ) then
					if( enc_filter_b_high(i) < 3 ) then
						enc_filter_b_high(i) := enc_filter_b_high(i) + 1;
					end if;
					enc_filter_b_low(i) := 0;
				end if;
				
				if( enc_filter_b_high(i) = 3 ) then
					enc_b_filtered(i) := '1';
				end if;
				
				if( ENC_B(i) = '0' ) then
					if( enc_filter_b_low(i) < 3 ) then
						enc_filter_b_low(i) := enc_filter_b_low(i) + 1;
					end if;
					enc_filter_b_high(i) := 0;
				end if;
				
				if( enc_filter_b_low(i) = 3 ) then
					enc_b_filtered(i) := '0';
				end if;
			end loop;
			
		end if;
		
		for i in 0 to 3 loop
			p_a(i) := enc_a_filtered(i) xor a_pre(i);
			p_b(i) := enc_b_filtered(i) xor b_pre(i);
			a_pre(i) := enc_a_filtered(i);
			b_pre(i) := enc_b_filtered(i);
				
			if( p_a(i) = '1' ) then
				if( enc_a_filtered(i) = '1' ) then 
					state(i) := A1;
				else
					state(i) := A0;
				end if;
			elsif( p_b(i) = '1' ) then
				if( enc_b_filtered(i) = '1' ) then 
					state(i) := B1;
				else
					state(i) := B0;
				end if;
			end if;
			
			case state(i) is
				when A1 =>
					case state_pre(i) is
						when A0 => --Richtungsänderung
							if( enc_b_filtered(i) = '1' ) then
								-- GUZS
								down(i) := down(i) + 1;									
							else
									-- UZS
								up(i) := up(i) + 1;
							end if;
							
							when B0 => 
								-- UZS
								up(i) := up(i) + 1;
								
							when B1 => 
								-- GUZS
								down(i) := down(i) + 1;									
								
							when others =>
							
						end case;
						state_pre(i) := A1;
						
				when A0 =>
					case state_pre(i) is
						when A1 => --Richtungsänderung
							if( enc_b_filtered(i) = '0' ) then
								-- GUZS
								down(i) := down(i) + 1;									
							else
									-- UZS
								up(i) := up(i) + 1;
							end if;
								
						when B1 => 
							-- UZS
							up(i) := up(i) + 1;
								
						when B0 => 
							-- GUZS
							down(i) := down(i) + 1;									
								
						when others =>
								
					end case;
					state_pre(i) := A0;
						
				when B1 =>
					case state_pre(i) is
						when B0 => --Richtungsänderung
							if( enc_a_filtered(i) = '0' ) then
								-- GUZS
								down(i) := down(i) + 1;									
							else
								-- UZS
								up(i) := up(i) + 1;
							end if;
								
						when A1 => 
							-- UZS
							up(i) := up(i) + 1;
								
						when A0 => 
							-- GUZS
							down(i) := down(i) + 1;									
								
						when others =>
								
					end case;
					state_pre(i) := B1;						
					
				when B0 =>
					case state_pre(i) is
						when B1 => --Richtungsänderung
							if( enc_a_filtered(i) = '1' ) then
								-- GUZS
								down(i) := down(i) + 1;									
							else
								-- UZS
								up(i) := up(i) + 1;
							end if;
								
						when A0 => 
							-- UZS
							up(i) := up(i) + 1;
								
						when A1 => 
							-- GUZS
							down(i) := down(i) + 1;
								
						when others =>
							
					end case;
					state_pre(i) := B0;
						
				when others =>
						
			end case;
		end loop;
				
	end if;
end if;
end process;

end Behavioral;


