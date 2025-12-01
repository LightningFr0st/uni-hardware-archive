----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 04:13:08 PM
-- Design Name: 
-- Module Name: dut_comb - Behavioral
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

entity dut_comb is
    Port (
        X : in  STD_LOGIC_VECTOR(2 downto 0);
        Y : out STD_LOGIC                     
    );
end dut_comb;

architecture Behavioral of dut_comb is
begin
    Y <= (X(0) and X(1)) xor X(2);
end Behavioral;
