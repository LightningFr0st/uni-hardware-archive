----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:36:44 PM
-- Design Name: 
-- Module Name: demux_1x4_beh - Behavioral
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

entity demux_1x4_beh is
    Port ( I : in STD_LOGIC;
       S : in STD_LOGIC_VECTOR(1 downto 0);
       D : out STD_LOGIC_VECTOR(3 downto 0));
end demux_1x4_beh;

architecture Behavioral of demux_1x4_beh is

begin
    D(0) <= I when (S(1) = '0' and S(0) = '0') else '0';
    D(1) <= I when (S(1) = '0' and S(0) = '1') else '0';
    D(2) <= I when (S(1) = '1' and S(0) = '0') else '0';
    D(3) <= I when (S(1) = '1' and S(0) = '1') else '0';
end Behavioral;
