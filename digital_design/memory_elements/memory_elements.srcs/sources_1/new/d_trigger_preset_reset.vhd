----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 02:49:32 PM
-- Design Name: 
-- Module Name: d_trigger_preset_reset - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity d_trigger_preset_reset is
    Port ( D : in STD_LOGIC;
   E : in STD_LOGIC;
   PRE : in STD_LOGIC;
   RST : in STD_LOGIC;
   Q : out STD_LOGIC;
   nQ : out STD_LOGIC);
end d_trigger_preset_reset;

architecture Behavioral of d_trigger_preset_reset is

signal state : STD_LOGIC := '0';

begin
    process(D, E, PRE, RST)
    begin
        if RST = '1' then
            state <= '0';
        elsif rising_edge(E) then
            if PRE = '1' then
                state <= '1';
            else
                state <= D;
        end if;
    end if;
    end process;
    Q <= state;
    nQ <= not state;
end Behavioral;
