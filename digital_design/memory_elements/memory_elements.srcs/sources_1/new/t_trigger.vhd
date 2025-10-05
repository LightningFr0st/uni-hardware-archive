----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 03:40:27 PM
-- Design Name: 
-- Module Name: t_trigger - Behavioral
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

entity t_trigger is
    Port ( D : in STD_LOGIC;
           C : in STD_LOGIC;
           Q : out STD_LOGIC);
end t_trigger;

architecture Behavioral of t_trigger is

signal state : STD_LOGIC := '1';

begin
    process(C)
    begin
        if rising_edge(C) then
            if D = '0' then
                state <= state;
            elsif D = '1' then
                state <= not state;
            end if;
        end if;
    end process;
    Q <= state;
end Behavioral;
