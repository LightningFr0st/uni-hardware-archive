----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 06:06:52 PM
-- Design Name: 
-- Module Name: frequency_divider - Structural
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

entity frequency_divider is
    Port (C : in STD_LOGIC;
       Q : out STD_LOGIC);
end frequency_divider;

architecture Structural of frequency_divider is

component t_trigger is
    Port ( D : in STD_LOGIC;
           C : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

signal c_div_2 : std_logic;

begin
    T_0: t_trigger port map(D=>'1', C=>C, Q=>c_div_2);
    T_1: t_trigger port map(D=>'1', C=>c_div_2, Q=>Q);
end Structural;
