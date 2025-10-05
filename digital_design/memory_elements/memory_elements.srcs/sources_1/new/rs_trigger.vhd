----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 03:33:08 PM
-- Design Name: 
-- Module Name: rs_trigger - Behavioral
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

entity rs_trigger is
    Port ( R : in STD_LOGIC;
           S : in STD_LOGIC;
           C : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end rs_trigger;

architecture Behavioral of rs_trigger is

signal state : std_logic;

begin
    process(C)
    begin
        if rising_edge(C) then
            if R = '1' and S = '0' then
                state <= '0';
            elsif R = '0' and S = '1' then
                state <= '1';
            elsif R = '1' and S = '1' then
                state <= 'X';
            end if;
        end if;
    end process;
end Behavioral;
