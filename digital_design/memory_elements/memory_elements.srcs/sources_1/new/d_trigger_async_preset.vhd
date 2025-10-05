----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 02:46:05 PM
-- Design Name: 
-- Module Name: d_trigger_async_preset - Behavioral
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

entity d_trigger_async_preset is
    Port ( D : in STD_LOGIC;
       E : in STD_LOGIC;
       PRE : in STD_LOGIC;
       Q : out STD_LOGIC;
       nQ : out STD_LOGIC);
end d_trigger_async_preset;

architecture Behavioral of d_trigger_async_preset is

signal state : STD_LOGIC := '0';

begin
    process(E, PRE)
    begin
        if PRE = '1' then
            state <= '1';
        elsif rising_edge(E) then
            state <= D;
        end if;
    end process;
    Q <= state;
    nQ <= not state;
end Behavioral;
