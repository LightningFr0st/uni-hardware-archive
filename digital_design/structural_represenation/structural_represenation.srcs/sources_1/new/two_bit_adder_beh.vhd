----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 11:39:22 PM
-- Design Name: 
-- Module Name: two_bit_adder_beh - Behavioral
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity two_bit_adder_beh is
    Port (
    A    : in  STD_LOGIC_VECTOR(1 downto 0);
    B    : in  STD_LOGIC_VECTOR(1 downto 0);
    Cin  : in  STD_LOGIC;
    S    : out STD_LOGIC_VECTOR(1 downto 0);
    Cout : out STD_LOGIC
);
end two_bit_adder_beh;

architecture Behavioral of two_bit_adder_beh is

begin
    process(A, B, Cin)
    variable temp : UNSIGNED(2 downto 0);
    begin
        temp := UNSIGNED('0' & A) + UNSIGNED('0' & B) + ('0' & Cin);
        S <= STD_LOGIC_VECTOR(temp(1 downto 0));
        Cout <= temp(2);
    end process;
end Behavioral;
